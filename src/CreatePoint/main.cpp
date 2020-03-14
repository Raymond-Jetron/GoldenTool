#include <cstdio>
#include <iostream>
#include <vector>
#include <mutex>
#include <math.h>
#include <queue>
#include <csignal>
#include <string.h>
#include <regex>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/null_sink.h"
#include "spdlog/details/file_helper.h"
#include "spdlog/details/os.h"
#include "spdlog/async.h"

#include "CLI/CLI.hpp"
#include "goldenapi.h"
#include "common.h"
#include "scope_guard.h"
#include "thread_pool.h"

using namespace std;
namespace spd = spdlog;
typedef std::shared_ptr<spdlog::logger> plog;

namespace golden_config
{
	std::string task_name_;
	std::vector<std::string> host_name_;
	int port_;
	std::string user_;
	std::string password_;
	int thread_count_;
	std::string points_dir_;
	bool print_log_;
	int log_level_;
	std::string ecoding_;
	std::string result_file_;

	int start_time_int_;
	short start_time_ms_;
	int end_time_int_;
	short end_time_ms_;
};

// 连接池
queue<golden_int32> connect_queue;
std::mutex cq_mutex;

// 日志
std::shared_ptr<spdlog::logger> g_logger = nullptr;
std::shared_ptr<spdlog::logger> g_progress = nullptr;

std::string g_app_root_path;
std::string app_name = "create_point";
bool g_running = false;
static thread_pool g_thread_pool;
#ifdef _WIN32
std::string g_to_charset = "gb2312";
#elif _LINUX
std::string g_to_charset = "utf-8";
#endif

// 数据库
std::vector<int> g_ids;
std::map<std::string, GOLDEN_TABLE> tables_info_in_db;

std::vector<std::string> points_files;

std::unordered_map<std::string, std::function<void(char*, GOLDEN_POINT*, GOLDEN_SCAN_POINT*, GOLDEN_CALC_POINT*)>> parse_func_map;
golden_byte g_on = 0x1;
golden_byte g_off = 0x0;
bool update_point_flow = false;  // 如果有id列，则默认id为准进入更新流程，否则以名称为准进入建点流程，若存在则更新

template<typename EnumType>
struct SEnumName
{
	static const char* List[];
};

template<> const char* SEnumName<GOLDEN_TYPE>::List[] =
{
	"BOOL",					// GOLDEN_BOOL = 0,        //!< 布尔类型，0值或1值。
	"UINT8",				// GOLDEN_UINT8 = 1,       //!< 无符号8位整数，占用1字节。
	"INT8",					// GOLDEN_INT8 = 2,        //!< 有符号8位整数，占用1字节。
	"CHAR",					// GOLDEN_CHAR = 3,        //!< 单字节字符，占用1字节。
	"UINT16",				// GOLDEN_UINT16 = 4,      //!< 无符号16位整数，占用2字节。
	"INT16",				// GOLDEN_INT16 = 5,       //!< 有符号16位整数，占用2字节。
	"UINT32",				// GOLDEN_UINT32 = 6,      //!< 无符号32位整数，占用4字节。
	"INT32",				// GOLDEN_INT32 = 7,       //!< 有符号32位整数，占用4字节。
	"INT64",				// GOLDEN_INT64 = 8,       //!< 有符号64位整数，占用8字节。
	"FLOAT16",				// GOLDEN_REAL16 = 9,      //!< 16位浮点数，占用2字节。
	"FLOAT32",				// GOLDEN_REAL32 = 10,     //!< 32位单精度浮点数，占用4字节。
	"FLOAT64",				// GOLDEN_REAL64 = 11,     //!< 64位双精度浮点数，占用8字节。
	"COOR",					// GOLDEN_COOR = 12,       //!< 二维坐标，具有x、y两个维度的浮点数，占用8字节。
	"STRING",				// GOLDEN_STRING = 13,     //!< 字符串，长度不超过存储页面大小。
	"BLOB",					// GOLDEN_BLOB = 14,       //!< 二进制数据块，占用字节不超过存储页面大小。
	"NAMED_T",				// GOLDEN_NAMED_T = 15,    //!< 自定义类型，由用户创建时确定字节长度。
	"DATETIME"				// GOLDEN_DATETIME = 16,   //!< 时间格式类型
};

enum GOLDEN_CLASSOF
{
	BASE = 0,      //!< 基本标签点，所有类别标签点均在基本标签点的属性集上扩展自己的属性集。
	SCAN = 1,      //!< 采集标签点。
	CALC = 2,      //!< 计算标签点。
	SCAN_CALC = 3  //!< 采集计算标签点。
};

template<> const char* SEnumName<GOLDEN_CLASSOF>::List[] =
{
	"base",           // BASE = 0,      //!< 基本标签点，所有类别标签点均在基本标签点的属性集上扩展自己的属性集。
	"base,scan",      // SCAN = 1,      //!< 采集标签点。
	"base,calc",      // CALC = 2,      //!< 计算标签点。
	"base,scan,calc"  // SCAN_CALC = 3, //!< 采集计算标签点。
};

//////////////////////////////工具函数///////////////////////////////
// 检查错误码
void check_ecode(golden_error ecode, const char *fun_name, plog& logger)
{
	if (GoE_C_ERRNO_ERROR < ecode) {
		logger->error("Failed to connect to the database, make sure the database is started or the connection parameters are correct.");
	}
	else {
		char ename[256] = { 0 };
		char emessage[256] = { 0 };
		go_format_message(ecode, emessage, ename, 256);
		logger->error("{}: error code: 0x{:X}, error name:{}, error desc:{}", fun_name, ecode, ename, emessage);
	}
}
#define CHECK_ECODE(ecode, func_name, logger) if (ecode != GoE_OK) check_ecode(ecode, func_name, logger);

template<typename EnumType>
EnumType ConvertStringToEnum(const char* pStr)
{
	EnumType fooEnum = static_cast<EnumType>(0);
	unsigned int count = (unsigned int)(sizeof(SEnumName<EnumType>::List) / sizeof(SEnumName<EnumType>::List[0]));
	for (unsigned int i = 0; i < count; ++i)
	{
		if (!abs(strcmp(pStr, SEnumName<EnumType>::List[i])))
		{
			fooEnum = static_cast<EnumType>(i);
			break;
		}
	}
	return fooEnum;
}
template<typename EnumType>
const char * ConvertEnumToString(EnumType enumPara)
{
	return SEnumName<EnumType>::List[enumPara];
}

// 初始化标签点属性
void init_point_prop(GOLDEN_POINT* base, GOLDEN_SCAN_POINT* scan, GOLDEN_CALC_POINT* calc)
{
	static GOLDEN_POINT base_default = {
		{ 0 },                       // char tag[GOLDEN_TAG_SIZE]  标签点名称
		0,                           // int id  全库唯一标识
		GOLDEN_TYPE::GOLDEN_REAL32,  // int type  标签点的数值类型
		1,                           // int table  标签点所属表ID
		{ 0 },                       // char desc[GOLDEN_DESC_SIZE] 标签点的描述
		{ 0 },                       // char unit[GOLDEN_UNIT_SIZE] 工程单位
		(golden_byte)1,              // golden_byte archive 是否存档
		(short)(-5),                 // short digits 数值位数
		(golden_byte)0,              // golden_byte shutdown 停机状态字（Shutdown）
		0.f,                         // float lowlimit 量程下限
		100.f,                       // float highlimit 量程上限
		(golden_byte)0,              // golden_byte step 是否阶跃
		50.f,                        // float typical 典型值
		(golden_byte)1,              // golden_byte compress 是否压缩
		1.f,                         // float compdev 压缩偏差
		1.f,                         // float compdevpercent 压缩偏差百分比
		28800,                       // int comptimemax 最大压缩间隔
		0,                           // int comptimemin 最短压缩间隔
		0.5f,                        // float excdev 例外偏差
		0.5f,                        // float excdevpercent 例外偏差百分比
		600,                         // int exctimemax 最大例外间隔
		0,                           // int exctimemin 最短例外间隔
		GOLDEN_CLASS::GOLDEN_BASE,   // unsigned int classof 标签点类别
		0,                           // int changedate 标签点属性最后一次被修改的时间
		{ 0 },                       // char changer[GOLDEN_USER_SIZE] 标签点属性最后一次被修改的用户名
		0,                           // int createdate 标签点被创建的时间
		{ 0 },                       // char creator[GOLDEN_USER_SIZE] 标签点创建者的用户名
		(golden_byte)0,              // golden_byte mirror 镜像收发控制
		(golden_byte)0,              // golden_byte millisecond 时间戳精度
		0,                           // unsigned int scanindex 采集点扩展属性集存储地址索引
		0,                           // unsigned int calcindex 计算点扩展属性集存储地址索引
		0,                           // unsigned int alarmindex 报警点扩展属性集存储地址索引
		{ 0 },                       // char table_dot_tag[GOLDEN_TAG_SIZE + GOLDEN_TAG_SIZE] 标签点全名
		(golden_byte)0,              // golden_byte summary 统计加速
		(golden_uint16)0,            // golden_uint16 named_type_id 标签点对应自定义类型id
	    { 0 }                        // golden_byte padding[GOLDEN_PACK_OF_POINT] 基本标签点备用字节
	};
	static GOLDEN_SCAN_POINT scan_default = {
		0,                           // int id 全库唯一标识
	    { 0 },                       // char source[GOLDEN_SOURCE_SIZE] 数据源
	    (golden_byte)1,              // golden_byte scan 是否采集
		{ 0 },                       // char instrument[GOLDEN_INSTRUMENT_SIZE] 设备标签
		{ 0, 0, 0, 0, 0 },           // int locations[GOLDEN_LOCATIONS_SIZE] 共包含五个设备位址
		{ 0, 0 },                    // int userints[GOLDEN_USERINT_SIZE] 共包含两个自定义整数
		{ 0.f, 0.f },                // float userreals[GOLDEN_USERREAL_SIZE] 共包含两个自定义单精度浮点数
		{ 0 }                        // golden_byte padding[GOLDEN_PACK_OF_SCAN] 采集标签点备用字节
	};
	static GOLDEN_CALC_POINT calc_default = {
		0,                           // int id 全库唯一标识
		{ 0 },                       // char equation[GOLDEN_EQUATION_SIZE] 实时方程式
		GOLDEN_TRIGGER::GOLDEN_NULL_TRIGGER, // golden_byte trigger 计算触发机制
		(golden_byte)0,              // golden_byte timecopy 计算结果时间戳参考
		1                            // int period 计算周期
	};
	
	memcpy(base, &base_default, sizeof(GOLDEN_POINT));
	memcpy(scan, &scan_default, sizeof(GOLDEN_SCAN_POINT));
	memcpy(calc, &calc_default, sizeof(GOLDEN_CALC_POINT));
}

// 获取文件夹下所有标签点文件
size_t get_points_files(std::string points_dir)
{
	if (points_dir.length())
	{
#ifdef _WIN32
		WIN32_FIND_DATA fd;
		points_dir += (points_dir.rfind('\\') == 0) ? "" : "\\";
		std::string find_points_filter = points_dir + "*.csv";
		HANDLE hFind = FindFirstFile(find_points_filter.c_str(), &fd);
		if (hFind == INVALID_HANDLE_VALUE)
		{
			g_logger->error("遍历标签点所在文件夹没有找到符合条件的文件");
			return 0;
		}
		do
		{
			if (fd.cFileName[0] != '.')
				points_files.push_back(points_dir + fd.cFileName);
		} while (FindNextFile(hFind, &fd));
		FindClose(hFind);
		return points_files.size();
#else
		DIR* dir = opendir(points_dir.c_str());      //打开指定目录  
		dirent* p = NULL;                            //定义遍历指针  
		while ((p = readdir(dir)) != NULL)           //开始逐个遍历  
		{
			//这里需要注意，linux平台下一个目录中有"."和".."隐藏文件，需要过滤掉  
			if (p->d_name[0] != '.')//d_name是一个char数组，存放当前遍历到的文件名  
			{
				if (strstr(p->d_name, ".csv") != NULL) {
					std::string name = points_dir + PATH_SEP + string(p->d_name);
					points_files.push_back(name);
				}
			}
		}
		closedir(dir);//关闭指定目录
		return points_files.size();
#endif // _WIN32
	}
	return 0;
}

// 准备连接池
void prepare_connections()
{
	golden_error ecode = GoE_OK;

	auto func_connect = [&](const char* host_name)->golden_int32
	{
		bool login_success = false;
		int db_handle = 0, priv = 0;
		while (g_running && (login_success == false))
		{
			if (GoE_OK == (ecode = go_connect(host_name, golden_config::port_, &db_handle)))
				if (GoE_OK == (ecode = go_login(db_handle, golden_config::user_.c_str(), golden_config::password_.c_str(), &priv)))
					login_success = true;
			if (!login_success)
			{
				this_thread::sleep_for(std::chrono::seconds(3));
				db_handle = 0;
				login_success = false;
				CHECK_ECODE(ecode, "Connect golden rtdb", g_logger);
			}
		}
		g_logger->trace("Connect golden rtdb host name : {}.", host_name);
		return db_handle;
	};

	// 不能大于单个客户端允许最大连接数
	golden_uint32 conntion_count_per_client = 0;
	int db_handle = func_connect(golden_config::host_name_.at(0).c_str());
	if (ecode != GoE_OK) return;
	ecode = go_get_db_info2(db_handle, GOLDEN_PARAM_ONE_CLINET_MAX_CONNECTION_COUNT, &conntion_count_per_client);
	CHECK_ECODE(ecode, "Get one client max connection count", g_logger);
	if (ecode != GoE_OK) return;
	if (golden_config::thread_count_ >= (int)conntion_count_per_client)
	{
		g_logger->warn("The number of threads is too large and the number of connections required exceeds the maximum number of connections allowed by a single client, and the maximum can be set to {}.", conntion_count_per_client - 1);
		golden_config::thread_count_ = conntion_count_per_client - 1;
	}
	connect_queue.push(db_handle);
	int host_count = (int)golden_config::host_name_.size();
	for (int i = 1; i < golden_config::thread_count_; ++i)
	{
		connect_queue.push(func_connect(golden_config::host_name_.at(i % host_count).c_str()));
	}
	g_logger->trace("Connection count is {}.", connect_queue.size());
}

// 释放连接池
void release_conncetions()
{
	while (!connect_queue.empty())
	{
		go_disconnect(connect_queue.front());
		connect_queue.pop();
	}
}

// 初始化日志
bool init_log()
{
	try {
		auto log_root_path = g_app_root_path + "/" + "log" + "/" + golden_config::task_name_;
		fmt::print("Log root path:{}\n", log_root_path);
		if (0 != creat_dir(log_root_path.c_str())) {
			throw std::runtime_error("create dir failed!");
		}
		auto log_file_ = log_root_path + "/" + app_name + "_logger.csv";
		fmt::print("Open log file {}\n", log_file_);
		// 输出到文件
		auto file_logger_ = std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_file_, true);
		assert(file_logger_);
		// 输出到控制台
		auto stdout_logger_ = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		assert(stdout_logger_);

		// 打印进度
		spdlog::sinks_init_list logger_list = { file_logger_, stdout_logger_ };
		g_progress = std::make_shared<spdlog::logger>(golden_config::task_name_, std::move(logger_list));
		g_progress->set_pattern("%^%t,progress,\"%v\"%$");
		g_progress->set_level(spdlog::level::level_enum::info);

		// 打印日志
		if (golden_config::print_log_) {
			logger_list = { file_logger_, stdout_logger_ };
			g_logger = std::make_shared<spdlog::logger>(golden_config::task_name_, std::move(logger_list));
		}
		else {
			g_logger = std::make_shared<spdlog::logger>(golden_config::task_name_, std::move(file_logger_));
		}
		g_logger->set_pattern("%t,%^%l%$,\"%v\"");
		g_logger->set_level(spdlog::level::level_enum(golden_config::log_level_));
		g_logger->info("Init log success.");
		return true;
	}
	catch (const spd::spdlog_ex& e) {
		std::cout << e.what() << std::endl;
		system("pause");
		return false;
	}
}

// 释放日志
void release_log()
{
	g_logger->flush();
}

// 检查参数
bool check_paramters()
{
	try
	{
		// thread count
		if (golden_config::thread_count_ < 1) {
			throw std::invalid_argument("thread count must >= 1.");
		}

		// host name
		if (golden_config::host_name_.empty()) {
			golden_config::host_name_.push_back("127.0.0.1");
		}
	}
	catch (std::invalid_argument &ia) {
		g_logger->error("invalid argument : {}", ia.what());
		return false;
	}
	return true;
}

// 初始化数据
bool prepare_metadata()
{
	// 构造解析函数全集
	//名称, 数值类型, 描述, 工程单位, 存档, 数值位数, 停机补写, 量程下限, 量程上限, 阶跃, 典型值, 压缩, 压缩偏差, 压缩百分比, 最长压缩间隔, 最短压缩间隔, 例外偏差, 例外百分比, 最长例外间隔, 最短例外间隔, 类别, 修改时间, 修改者, 创建时间, 创建者, 镜像, 时间戳精度, 加速统计
	parse_func_map[std::string("名称")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		// 去掉中括号
		memcpy(base->tag, field + 1, GOLDEN_MIN(GOLDEN_TAG_SIZE, strlen(field) - 2));
	};
	parse_func_map[std::string("数值类型")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		base->type = ConvertStringToEnum<GOLDEN_TYPE>(field);
	};
	parse_func_map[std::string("描述")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		memcpy(base->desc, field, GOLDEN_MIN(GOLDEN_DESC_SIZE, strlen(field)));
	};
	parse_func_map[std::string("工程单位")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		memcpy(base->unit, field, GOLDEN_MIN(GOLDEN_UNIT_SIZE, strlen(field)));
	};
	parse_func_map[std::string("存档")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		if (atoi(field) != 0) memcpy(&(base->archive), &g_on, 1);
	};
	parse_func_map[std::string("数值位数")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		base->digits = (short)atoi(field);
	};
	parse_func_map[std::string("停机补写")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		if (atoi(field) != 0) memcpy(&(base->shutdown), &g_on, 1);
	};
	parse_func_map[std::string("量程下限")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		base->lowlimit = (float)atof(field);
	};
	parse_func_map[std::string("量程上限")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		base->highlimit = (float)atof(field);
	};
	parse_func_map[std::string("阶跃")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		if (atoi(field) != 0) memcpy(&(base->step), &g_on, 1);
	};
	parse_func_map[std::string("典型值")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		base->typical = (float)atof(field);
	};
	parse_func_map[std::string("压缩")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		if (atoi(field) != 0) memcpy(&(base->compress), &g_on, 1);
	};
	parse_func_map[std::string("压缩偏差")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		base->compdev = (float)atof(field);
	};
	parse_func_map[std::string("压缩百分比")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		base->compdevpercent = (float)atof(field);
	};
	parse_func_map[std::string("最长压缩间隔")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		base->comptimemax = atoi(field);
	};
	parse_func_map[std::string("最短压缩间隔")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		base->comptimemin = atoi(field);
	};
	parse_func_map[std::string("例外偏差")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		base->excdev = (float)atof(field);
	};
	parse_func_map[std::string("例外百分比")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		base->excdevpercent = (float)atof(field);
	};
	parse_func_map[std::string("最长例外间隔")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		base->exctimemax = atoi(field);
	};
	parse_func_map[std::string("最短例外间隔")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		base->exctimemin = atoi(field);
	};
	parse_func_map[std::string("类别")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		base->classof = ConvertStringToEnum<GOLDEN_CLASSOF>(field);
	};
	parse_func_map[std::string("修改时间")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
	}; 
	parse_func_map[std::string("修改者")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
	}; 
	parse_func_map[std::string("创建时间")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
	}; 
	parse_func_map[std::string("创建者")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
	};
	parse_func_map[std::string("镜像")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		base->mirror = (golden_byte)atoi(field);
	};
	parse_func_map[std::string("时间戳精度")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		if (atoi(field) != 0) memcpy(&(base->millisecond), &g_on, 1);
	};
	parse_func_map[std::string("加速统计")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		if (atoi(field) != 0) memcpy(&(base->summary), &g_on, 1);
	};
	// 数据源, 采集, 设备标签, LOCATION1, LOCATION2, LOCATION3, LOCATION4, LOCATION5, USERINT1, USERINT2, USERREAL1, USERREAL2
	parse_func_map[std::string("数据源")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		memcpy(scan->source, field, GOLDEN_MIN(GOLDEN_SOURCE_SIZE, strlen(field)));
	};
	parse_func_map[std::string("采集")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		if (atoi(field) != 0) memcpy(&(scan->scan), &g_on, 1);
	};
	parse_func_map[std::string("设备标签")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		memcpy(scan->instrument, field, GOLDEN_MIN(GOLDEN_INSTRUMENT_SIZE, strlen(field)));
	};
	parse_func_map[std::string("LOCATION1")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		scan->locations[0] = atoi(field);
	};
	parse_func_map[std::string("LOCATION2")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		scan->locations[1] = atoi(field);
	};
	parse_func_map[std::string("LOCATION3")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		scan->locations[2] = atoi(field);
	};
	parse_func_map[std::string("LOCATION4")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		scan->locations[3] = atoi(field);
	};
	parse_func_map[std::string("LOCATION5")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		scan->locations[4] = atoi(field);
	};
	parse_func_map[std::string("USERINT1")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		scan->userints[0] = atoi(field);
	};
	parse_func_map[std::string("USERINT2")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		scan->userints[1] = atoi(field);
	};
	parse_func_map[std::string("USERREAL1")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		scan->userreals[0] = (float)atof(field);
	};
	parse_func_map[std::string("USERREAL2")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		scan->userreals[1] = (float)atof(field);
	};
	// 方程式, 触发方式, 时间戳参考, 计算周期
	parse_func_map[std::string("方程式")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		memcpy(calc->equation, field, GOLDEN_MIN(GOLDEN_EQUATION_SIZE, strlen(field)));
	};
	parse_func_map[std::string("触发方式")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		calc->trigger = (golden_byte)atoi(field);
	};
	parse_func_map[std::string("时间戳参考")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		calc->timecopy = (golden_byte)atoi(field);
	};
	parse_func_map[std::string("计算周期")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		calc->period = atoi(field);
	};
	// id，如果有这一列，则以ID为准更新标签点属性，包括名称
	parse_func_map[std::string("id")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		base->id = atoi(field);
	};

	// 读取数据库元数据信息
	golden_error ecode = GoE_OK;
	int nHanle = 0;
	{
		// 从连接池获取连接句柄
		std::unique_lock<std::mutex> lock(cq_mutex);
		g_logger->trace("Get connection handle, current connection pool size : {}", connect_queue.size());
		nHanle = connect_queue.front();
		connect_queue.pop();
	}
	ON_SCOPE_EXIT([&] {
		if (nHanle) {
			std::unique_lock<std::mutex> lock(cq_mutex);
			connect_queue.push(nHanle);
			g_logger->trace("Return connection handle : {}, current connection pool size : {}", nHanle, connect_queue.size());
		}
	});

	// 获取表个数
	int tables_count = 0;
	ecode = gob_tables_count(nHanle, &tables_count);
	if (ecode != GoE_OK) {
		check_ecode(ecode, "Get tables count", g_logger);
		return false;
	}
	g_logger->info("Get [{}] tables in database", tables_count);

	if (0 == tables_count)
		return true;

	// 获取表ID
	vector<int> tables_id(tables_count);
	ecode = gob_get_tables(nHanle, tables_id.data(), &tables_count);
	if (ecode != GoE_OK) {
		check_ecode(ecode, "Get tables id", g_logger);
		return false;
	}

	// 获取所有表信息
	GOLDEN_TABLE table_info_temp;
	for (int table_id : tables_id) {
		table_info_temp.id = table_id;
		memset(table_info_temp.name, 0, GOLDEN_TAG_SIZE);
		memset(table_info_temp.desc, 0, GOLDEN_DESC_SIZE);
		ecode = gob_get_table_property_by_id(nHanle, &table_info_temp);
		tables_info_in_db.insert({ table_info_temp.name, table_info_temp });
		g_logger->info("Get table [{}] info : id=[{}], desc={}", table_info_temp.name, table_info_temp.id, table_info_temp.desc);
	}

	return true;
}

// 创建测点
bool create_point() 
{
	auto create_point_func = [&](const std::string &point_file_path) {
		// 打开文件
		input_file ifile(point_file_path);
		auto& ifs = ifile.ifs();
		ifs.seekg(ifstream::beg);
		std::string title_line;
		getline(ifs, title_line);

		//code_converter cc = code_converter(golden_config::ecoding_.c_str(), g_to_charset.c_str());
		//code_converter cc = code_converter("gb2312", "utf-8");
		update_point_flow = false;

		golden_error ecode = GoE_OK;
		int nHanle = 0;
		{
			// 从连接池获取连接句柄
			std::unique_lock<std::mutex> lock(cq_mutex);
			g_logger->trace("Get connection handle, current connection pool size : {}", connect_queue.size());
			nHanle = connect_queue.front();
			connect_queue.pop();
		}
		ON_SCOPE_EXIT([&] {
			if (nHanle) {
				std::unique_lock<std::mutex> lock(cq_mutex);
				connect_queue.push(nHanle);
				g_logger->trace("Return connection handle : {}, current connection pool size : {}", nHanle, connect_queue.size());
			}
		});

		std::string table_name(strrchr(point_file_path.c_str(), PATH_SEP) + 1, strrchr(point_file_path.c_str(), '.') - strrchr(point_file_path.c_str(), PATH_SEP) - 1);

		// 检查表是否存在，不存在则创建
		auto table_info_it = tables_info_in_db.find(table_name);
		if (table_info_it == tables_info_in_db.end()) {
			GOLDEN_TABLE table_info = { 0 };
			memcpy(table_info.name, table_name.c_str(), table_name.size());
			ecode = gob_append_table(nHanle, &table_info);
			if (ecode != GoE_OK) {
				check_ecode(ecode, fmt::format("Append table {}", table_name).c_str(), g_logger);
				return;
			}
			else {
				tables_info_in_db.insert({ table_name, table_info });
				table_info_it = tables_info_in_db.find(table_name);
				this_thread::sleep_for(std::chrono::seconds(1));
			}
		}

		// 存放一行的各个字段的指针
		std::vector<char*> fields_ptr;
		size_t field_index = 0;
		size_t id_field_number = 0;

		// 根据标题行定义解析顺序
		// 提取标题行每个字段的指针
		char* p = const_cast<char*>(title_line.c_str());
		char* p_end = p + title_line.size();
		do {
			fields_ptr.push_back(p);
			while (*p != '\r' && *p != '\n' && *p != ',') ++p;
			*p++ = '\0';
		}while(p < p_end);
		// 构造每个字段的解析方法
		std::vector<std::function<void(char*, GOLDEN_POINT*, GOLDEN_SCAN_POINT*, GOLDEN_CALC_POINT*)>> line_parse_func;
		char out_buf[128];
		auto it_func = parse_func_map.end();
		for (field_index = 0; field_index < fields_ptr.size(); ++field_index) {
			if (strcmp(fields_ptr[field_index], "id") == 0) {
				update_point_flow = true;
				id_field_number = field_index;
			}
			it_func = parse_func_map.find(fields_ptr[field_index]);
			if (it_func != parse_func_map.end()) {
				line_parse_func.push_back(it_func->second);
			}
			else {
				line_parse_func.push_back([](char*, GOLDEN_POINT*, GOLDEN_SCAN_POINT*, GOLDEN_CALC_POINT*) {});
				//cc.convert(fields_ptr[field_index], strlen(fields_ptr[field_index]), out_buf, 128);
				//g_logger->error("Unrecognized fields {} : [{}].", field_index + 1, out_buf/*fields_ptr[field_index]*/);
				g_logger->error("Unrecognized fields {} : [{}].", field_index + 1, fields_ptr[field_index]);
			}
		}
		
		// 标签点属性
		GOLDEN_POINT base;
		GOLDEN_SCAN_POINT scan;
		GOLDEN_CALC_POINT calc;

		std::string line;
		size_t line_index = 2;
		while (getline(ifs, line))
		{
			fields_ptr.clear();
			char* p = const_cast<char*>(line.c_str());
			char* p_end = p + line.size();
			do {
				if (*p == '\"') {
					fields_ptr.push_back(++p);
					while (*p != '\"') ++p;
					*p = '\0';
					p += 2;
				}
				else {
					fields_ptr.push_back(p);
					while (*p != '\r' && *p != '\n' && *p != ',') ++p;
					*p++ = '\0';
				}
			} while (p < p_end);

			if (fields_ptr.size() != line_parse_func.size()) {
				g_logger->error("Parse failed, line number : {}, field count[={}] is not match with title[={}].", line_index, fields_ptr.size(), line_parse_func.size());
				return;
			}

			// 初始化标签点属性
			if (update_point_flow) {
				// 首先解析id
				line_parse_func[id_field_number](fields_ptr[id_field_number], &base, &scan, &calc);
				// 获取标签点属性
				golden_error errors = GoE_OK;
				ecode = gob_get_points_property(nHanle, 1, &base, &scan, &calc, &errors);
				if (ecode != GoE_OK) {
					check_ecode(ecode, fmt::format("Get point {}", base.tag).c_str(), g_logger);
					return;
				}
				g_logger->trace("Get point [{}.{}] succesful.", table_name, base.tag);
			}
			else {
				init_point_prop(&base, &scan, &calc);
			}

			// 解析点表填充
			try {
				for (field_index = 0; field_index < line_parse_func.size(); ++field_index) {
					line_parse_func[field_index](fields_ptr[field_index], &base, &scan, &calc);
				}
			}
			catch (...) {
				g_logger->error("Parse failed, line number : {}, field number : {}", line_index, field_index + 1);
				return;
			}

			if (update_point_flow) {
				ecode = gob_update_point_property(nHanle, &base, &scan, &calc);
				if (ecode != GoE_OK) {
					check_ecode(ecode, fmt::format("Update point {}", base.tag).c_str(), g_logger);
					return;
				}
				g_logger->trace("Update point [{}.{}] succesful.", table_name, base.tag);
			}
			else {
				base.table = table_info_it->second.id;
				ecode = gob_insert_point(nHanle, &base, &scan, &calc);
				// 如果标签点已经存在，则更新属性
				if (ecode == GoE_REDUPLICATE_TAG) {
					g_logger->warn("Point [{}.{}] already exists, update it's properties.", table_name, base.tag);
					int find_count = 1;
					auto&& table_dot_tag = table_info_it->first + "." + base.tag;
					const char* table_dot_tags[1] = { table_dot_tag.c_str() };
					ecode = gob_find_points(nHanle, &find_count, table_dot_tags, &base.id, nullptr, nullptr, nullptr);
					ecode = gob_update_point_property(nHanle, &base, &scan, &calc);
					if (ecode != GoE_OK) {
						check_ecode(ecode, fmt::format("Update point {}", base.tag).c_str(), g_logger);
						return;
					}
					g_logger->trace("Update point [{}.{}] succesful.", table_name, base.tag);
				}
				if (ecode != GoE_OK) {
					check_ecode(ecode, fmt::format("Create point {}", base.tag).c_str(), g_logger);
					return;
				}
				g_logger->trace("Create point [{}.{}] succesful.", table_name, base.tag);
			}
			//base.table = table_info_it->second.id;
			//if (base.id) {  // 指定ID，则更新属性
			//	ecode = gob_update_point_property(nHanle, &base, &scan, &calc);
			//	if (ecode != GoE_OK) {
			//		check_ecode(ecode, fmt::format("Update point {}", base.tag).c_str(), g_logger);
			//		return;
			//	}
			//	g_logger->trace("Update point [{}.{}] succesful.", table_name, base.tag);
			//}
			//else {
			//	ecode = gob_insert_point(nHanle, &base, &scan, &calc);
			//	// 如果标签点已经存在，则更新属性
			//	if (ecode == GoE_REDUPLICATE_TAG) {
			//		g_logger->warn("Point [{}.{}] already exists, update it's properties.", table_name, base.tag);
			//		int find_count = 1;
			//		auto&& table_dot_tag = table_info_it->first + "." + base.tag;
			//		const char* table_dot_tags[1] = { table_dot_tag.c_str() };
			//		ecode = gob_find_points(nHanle, &find_count, table_dot_tags, &base.id, nullptr, nullptr, nullptr);
			//		ecode = gob_update_point_property(nHanle, &base, &scan, &calc);
			//		if (ecode != GoE_OK) {
			//			check_ecode(ecode, fmt::format("Update point {}", base.tag).c_str(), g_logger);
			//			return;
			//		}
			//		g_logger->trace("Update point [{}.{}] succesful.", table_name, base.tag);
			//	}
			//	if (ecode != GoE_OK) {
			//		check_ecode(ecode, fmt::format("Create point {}", base.tag).c_str(), g_logger);
			//		return;
			//	}
			//	g_logger->trace("Create point [{}.{}] succesful.", table_name, base.tag);
			//}
			++line_index;
		}
	};

	size_t points_file_count = get_points_files(golden_config::points_dir_);
	if (points_file_count > 0) {
		g_logger->info("There are {} tasks to deal with.", points_file_count);
		std::thread thd;
		stop_watch watch;
		watch.start();
		for (size_t i = 0; i < (size_t)points_file_count; ++i) {
			// 跳过已经同步完成的点表
			if (points_files.at(i).find("[OK]") == std::string::npos) {
				thd = std::thread(create_point_func, points_files.at(i));
				thd.join();
			}
			else {
				g_logger->info("Task [{}] has been completed.", points_files.at(i));
			}
			g_progress->critical("Task {} just completed, progress is : {}%", i + 1, (i + 1) * 100 / points_file_count);
		}
		watch.stop();
		g_progress->critical("{} tasks have been completed, total elapsed is {}s", points_file_count, watch.elapsed_second());
	}
	return true;
}

void sig_handler(int sig)
{
	if (sig == SIGINT)
	{
		g_running = false;
	}
}

int main(int argc, char *argv[])
{
	std::cout << " ,--.             .       .-,--.           .  " << endl;
	std::cout << "| `-' ,-. ,-. ,-. |- ,-.   '|__/ ,-. . ,-. |- " << endl;
	std::cout << "|   . |   |-' ,-| |  |-'   ,|    | | | | | |  " << endl;
	std::cout << "`--'  '   `-' `-^ `' `-'   `'    `-' ' ' ' `' " << endl;
	std::cout << "                                              " << endl;
	std::cout << "                                              " << endl << endl;
	// http://patorjk.com/software/taag/#p=display&f=Stampatello&t=Golden%20Create%20Point

	CLI::App app{ "App description" };
	{
		golden_config::task_name_ = "main";
		app.add_option("-n,--taskname", golden_config::task_name_, "task name (=main)")->required();
		golden_config::host_name_.push_back("127.0.0.1");
		app.add_option("-a,--address", golden_config::host_name_, "host name (=127.0.0.1)\n You can set up multi-IP addresses separated by spaces to take advantage of the server's multi-network card.\n e.g. -a 192.168.0.2 192.168.0.3 192.168.0.4 192.168.0.5");
		golden_config::port_ = 6327;
		app.add_option("-p,--port", golden_config::port_, "port number (=6327)");
		golden_config::user_ = "sa";
		app.add_option("-u,--user", golden_config::user_, "user name (=sa)");
		golden_config::password_ = "admin";
		app.add_option("-w,--password", golden_config::password_, "pass word (=admin)");
		golden_config::thread_count_ = 1;
		app.add_option("--thread_count", golden_config::thread_count_, "thread count (=1)");
		app.add_option("--points_dir", golden_config::points_dir_, "point file directory (*.csv)")->required();
		golden_config::print_log_ = true;
		app.add_flag("--print_log", golden_config::print_log_, "print log to console");
		golden_config::log_level_ = spdlog::level::level_enum::info;
		app.add_option("--log_level", golden_config::log_level_, "log level (=2) as info\n 0.trace\n 1.debug\n 2.info\n 3.warn\n 4.err\n 5.critical\n 6.off");
		golden_config::ecoding_ = "gb2312";
		app.add_option("--ecoding", golden_config::ecoding_, "Encoding character sets (=gb2312)\n \"gb2312\" or \"utf-8\"");
		app.add_option("--result_file", golden_config::result_file_, "result file path, default is empty.");
		app.add_flag("--Attention", "Attention:\n1.Time range with milliseconds can be passed in.\n2.If multiple points are queried, they are automatically assigned to multiple query threads.");

		try {
			(app).parse((argc), (argv));
		}
		catch (const CLI::ParseError &e) {
			return (app).exit(e);
		}
	}

	g_app_root_path.append(argv[0], strrchr(argv[0], PATH_SEP) - argv[0]);
	fmt::print("App path : {}\n", g_app_root_path);
	fmt::print("Preparing for work, do not quit!\n");
	fmt::print("=============================\n");
	g_running = true;

	if (!init_log())
		return -1;
	ON_SCOPE_EXIT([&] {
		release_log();
	});

	g_logger->trace("Check paramters.");
	if (!check_paramters())
		return -1;

	g_logger->trace("Preparing for connections.");
	prepare_connections();
	ON_SCOPE_EXIT([&] {
		release_conncetions();
		g_logger->info("Release all connections.");
	});

	g_logger->trace("Preparing metadata.");
	if (!prepare_metadata())
		return -1;

	g_logger->trace("Creating point.");
	if (!create_point())
		return -1;

	g_logger->info("Exit the app.");
    return 0;
}