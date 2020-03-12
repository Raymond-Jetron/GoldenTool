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

// ���ӳ�
queue<golden_int32> connect_queue;
std::mutex cq_mutex;

// ��־
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

// ���ݿ�
std::vector<int> g_ids;
std::map<std::string, GOLDEN_TABLE> tables_info_in_db;

std::vector<std::string> points_files;

std::unordered_map<std::string, std::function<void(char*, GOLDEN_POINT*, GOLDEN_SCAN_POINT*, GOLDEN_CALC_POINT*)>> parse_func_map;
golden_byte g_on = 0x1;
golden_byte g_off = 0x0;
bool update_point_flow = false;  // �����id�У���Ĭ��idΪ׼����������̣�����������Ϊ׼���뽨�����̣������������

template<typename EnumType>
struct SEnumName
{
	static const char* List[];
};

template<> const char* SEnumName<GOLDEN_TYPE>::List[] =
{
	"BOOL",					// GOLDEN_BOOL = 0,        //!< �������ͣ�0ֵ��1ֵ��
	"UINT8",				// GOLDEN_UINT8 = 1,       //!< �޷���8λ������ռ��1�ֽڡ�
	"INT8",					// GOLDEN_INT8 = 2,        //!< �з���8λ������ռ��1�ֽڡ�
	"CHAR",					// GOLDEN_CHAR = 3,        //!< ���ֽ��ַ���ռ��1�ֽڡ�
	"UINT16",				// GOLDEN_UINT16 = 4,      //!< �޷���16λ������ռ��2�ֽڡ�
	"INT16",				// GOLDEN_INT16 = 5,       //!< �з���16λ������ռ��2�ֽڡ�
	"UINT32",				// GOLDEN_UINT32 = 6,      //!< �޷���32λ������ռ��4�ֽڡ�
	"INT32",				// GOLDEN_INT32 = 7,       //!< �з���32λ������ռ��4�ֽڡ�
	"INT64",				// GOLDEN_INT64 = 8,       //!< �з���64λ������ռ��8�ֽڡ�
	"FLOAT16",				// GOLDEN_REAL16 = 9,      //!< 16λ��������ռ��2�ֽڡ�
	"FLOAT32",				// GOLDEN_REAL32 = 10,     //!< 32λ�����ȸ�������ռ��4�ֽڡ�
	"FLOAT64",				// GOLDEN_REAL64 = 11,     //!< 64λ˫���ȸ�������ռ��8�ֽڡ�
	"COOR",					// GOLDEN_COOR = 12,       //!< ��ά���꣬����x��y����ά�ȵĸ�������ռ��8�ֽڡ�
	"STRING",				// GOLDEN_STRING = 13,     //!< �ַ��������Ȳ������洢ҳ���С��
	"BLOB",					// GOLDEN_BLOB = 14,       //!< ���������ݿ飬ռ���ֽڲ������洢ҳ���С��
	"NAMED_T",				// GOLDEN_NAMED_T = 15,    //!< �Զ������ͣ����û�����ʱȷ���ֽڳ��ȡ�
	"DATETIME"				// GOLDEN_DATETIME = 16,   //!< ʱ���ʽ����
};

enum GOLDEN_CLASSOF
{
	BASE = 0,      //!< ������ǩ�㣬��������ǩ����ڻ�����ǩ������Լ�����չ�Լ������Լ���
	SCAN = 1,      //!< �ɼ���ǩ�㡣
	CALC = 2,      //!< �����ǩ�㡣
	SCAN_CALC = 3  //!< �ɼ������ǩ�㡣
};

template<> const char* SEnumName<GOLDEN_CLASSOF>::List[] =
{
	"base",           // BASE = 0,      //!< ������ǩ�㣬��������ǩ����ڻ�����ǩ������Լ�����չ�Լ������Լ���
	"base,scan",      // SCAN = 1,      //!< �ɼ���ǩ�㡣
	"base,calc",      // CALC = 2,      //!< �����ǩ�㡣
	"base,scan,calc"  // SCAN_CALC = 3, //!< �ɼ������ǩ�㡣
};

//////////////////////////////���ߺ���///////////////////////////////
// ��������
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

// ��ʼ����ǩ������
void init_point_prop(GOLDEN_POINT* base, GOLDEN_SCAN_POINT* scan, GOLDEN_CALC_POINT* calc)
{
	static GOLDEN_POINT base_default = {
		{ 0 },                       // char tag[GOLDEN_TAG_SIZE]  ��ǩ������
		0,                           // int id  ȫ��Ψһ��ʶ
		GOLDEN_TYPE::GOLDEN_REAL32,  // int type  ��ǩ�����ֵ����
		1,                           // int table  ��ǩ��������ID
		{ 0 },                       // char desc[GOLDEN_DESC_SIZE] ��ǩ�������
		{ 0 },                       // char unit[GOLDEN_UNIT_SIZE] ���̵�λ
		(golden_byte)1,              // golden_byte archive �Ƿ�浵
		(short)(-5),                 // short digits ��ֵλ��
		(golden_byte)0,              // golden_byte shutdown ͣ��״̬�֣�Shutdown��
		0.f,                         // float lowlimit ��������
		100.f,                       // float highlimit ��������
		(golden_byte)0,              // golden_byte step �Ƿ��Ծ
		50.f,                        // float typical ����ֵ
		(golden_byte)1,              // golden_byte compress �Ƿ�ѹ��
		1.f,                         // float compdev ѹ��ƫ��
		1.f,                         // float compdevpercent ѹ��ƫ��ٷֱ�
		28800,                       // int comptimemax ���ѹ�����
		0,                           // int comptimemin ���ѹ�����
		0.5f,                        // float excdev ����ƫ��
		0.5f,                        // float excdevpercent ����ƫ��ٷֱ�
		600,                         // int exctimemax ���������
		0,                           // int exctimemin ���������
		GOLDEN_CLASS::GOLDEN_BASE,   // unsigned int classof ��ǩ�����
		0,                           // int changedate ��ǩ���������һ�α��޸ĵ�ʱ��
		{ 0 },                       // char changer[GOLDEN_USER_SIZE] ��ǩ���������һ�α��޸ĵ��û���
		0,                           // int createdate ��ǩ�㱻������ʱ��
		{ 0 },                       // char creator[GOLDEN_USER_SIZE] ��ǩ�㴴���ߵ��û���
		(golden_byte)0,              // golden_byte mirror �����շ�����
		(golden_byte)0,              // golden_byte millisecond ʱ�������
		0,                           // unsigned int scanindex �ɼ�����չ���Լ��洢��ַ����
		0,                           // unsigned int calcindex �������չ���Լ��洢��ַ����
		0,                           // unsigned int alarmindex ��������չ���Լ��洢��ַ����
		{ 0 },                       // char table_dot_tag[GOLDEN_TAG_SIZE + GOLDEN_TAG_SIZE] ��ǩ��ȫ��
		(golden_byte)0,              // golden_byte summary ͳ�Ƽ���
		(golden_uint16)0,            // golden_uint16 named_type_id ��ǩ���Ӧ�Զ�������id
	    { 0 }                        // golden_byte padding[GOLDEN_PACK_OF_POINT] ������ǩ�㱸���ֽ�
	};
	static GOLDEN_SCAN_POINT scan_default = {
		0,                           // int id ȫ��Ψһ��ʶ
	    { 0 },                       // char source[GOLDEN_SOURCE_SIZE] ����Դ
	    (golden_byte)1,              // golden_byte scan �Ƿ�ɼ�
		{ 0 },                       // char instrument[GOLDEN_INSTRUMENT_SIZE] �豸��ǩ
		{ 0, 0, 0, 0, 0 },           // int locations[GOLDEN_LOCATIONS_SIZE] ����������豸λַ
		{ 0, 0 },                    // int userints[GOLDEN_USERINT_SIZE] �����������Զ�������
		{ 0.f, 0.f },                // float userreals[GOLDEN_USERREAL_SIZE] �����������Զ��嵥���ȸ�����
		{ 0 }                        // golden_byte padding[GOLDEN_PACK_OF_SCAN] �ɼ���ǩ�㱸���ֽ�
	};
	static GOLDEN_CALC_POINT calc_default = {
		0,                           // int id ȫ��Ψһ��ʶ
		{ 0 },                       // char equation[GOLDEN_EQUATION_SIZE] ʵʱ����ʽ
		GOLDEN_TRIGGER::GOLDEN_NULL_TRIGGER, // golden_byte trigger ���㴥������
		(golden_byte)0,              // golden_byte timecopy ������ʱ����ο�
		1                            // int period ��������
	};
	
	memcpy(base, &base_default, sizeof(GOLDEN_POINT));
	memcpy(scan, &scan_default, sizeof(GOLDEN_SCAN_POINT));
	memcpy(calc, &calc_default, sizeof(GOLDEN_CALC_POINT));
}

// ��ȡ�ļ��������б�ǩ���ļ�
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
			g_logger->error("������ǩ�������ļ���û���ҵ������������ļ�");
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
		DIR* dir = opendir(points_dir.c_str());      //��ָ��Ŀ¼  
		dirent* p = NULL;                            //�������ָ��  
		while ((p = readdir(dir)) != NULL)           //��ʼ�������  
		{
			//������Ҫע�⣬linuxƽ̨��һ��Ŀ¼����"."��".."�����ļ�����Ҫ���˵�  
			if (p->d_name[0] != '.')//d_name��һ��char���飬��ŵ�ǰ���������ļ���  
			{
				if (strstr(p->d_name, ".csv") != NULL) {
					std::string name = points_dir + PATH_SEP + string(p->d_name);
					points_files.push_back(name);
				}
			}
		}
		closedir(dir);//�ر�ָ��Ŀ¼
		return points_files.size();
#endif // _WIN32
	}
	return 0;
}

// ׼�����ӳ�
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

	// ���ܴ��ڵ����ͻ����������������
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

// �ͷ����ӳ�
void release_conncetions()
{
	while (!connect_queue.empty())
	{
		go_disconnect(connect_queue.front());
		connect_queue.pop();
	}
}

// ��ʼ����־
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
		// ������ļ�
		auto file_logger_ = std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_file_, true);
		assert(file_logger_);
		// ���������̨
		auto stdout_logger_ = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		assert(stdout_logger_);

		// ��ӡ����
		spdlog::sinks_init_list logger_list = { file_logger_, stdout_logger_ };
		g_progress = std::make_shared<spdlog::logger>(golden_config::task_name_, std::move(logger_list));
		g_progress->set_pattern("%^%t,progress,\"%v\"%$");
		g_progress->set_level(spdlog::level::level_enum::info);

		// ��ӡ��־
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

// �ͷ���־
void release_log()
{
	g_logger->flush();
}

// ������
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

// ��ʼ������
bool prepare_metadata()
{
	// �����������ȫ��
	//����, ��ֵ����, ����, ���̵�λ, �浵, ��ֵλ��, ͣ����д, ��������, ��������, ��Ծ, ����ֵ, ѹ��, ѹ��ƫ��, ѹ���ٷֱ�, �ѹ�����, ���ѹ�����, ����ƫ��, ����ٷֱ�, �������, ���������, ���, �޸�ʱ��, �޸���, ����ʱ��, ������, ����, ʱ�������, ����ͳ��
	parse_func_map[std::string("����")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		// ȥ��������
		memcpy(base->tag, field + 1, GOLDEN_MIN(GOLDEN_TAG_SIZE, strlen(field) - 2));
	};
	parse_func_map[std::string("��ֵ����")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		base->type = ConvertStringToEnum<GOLDEN_TYPE>(field);
	};
	parse_func_map[std::string("����")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		memcpy(base->desc, field, GOLDEN_MIN(GOLDEN_DESC_SIZE, strlen(field)));
	};
	parse_func_map[std::string("���̵�λ")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		memcpy(base->unit, field, GOLDEN_MIN(GOLDEN_UNIT_SIZE, strlen(field)));
	};
	parse_func_map[std::string("�浵")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		if (atoi(field) != 0) memcpy(&(base->archive), &g_on, 1);
	};
	parse_func_map[std::string("��ֵλ��")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		base->digits = (short)atoi(field);
	};
	parse_func_map[std::string("ͣ����д")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		if (atoi(field) != 0) memcpy(&(base->shutdown), &g_on, 1);
	};
	parse_func_map[std::string("��������")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		base->lowlimit = (float)atof(field);
	};
	parse_func_map[std::string("��������")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		base->highlimit = (float)atof(field);
	};
	parse_func_map[std::string("��Ծ")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		if (atoi(field) != 0) memcpy(&(base->step), &g_on, 1);
	};
	parse_func_map[std::string("����ֵ")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		base->typical = (float)atof(field);
	};
	parse_func_map[std::string("ѹ��")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		if (atoi(field) != 0) memcpy(&(base->compress), &g_on, 1);
	};
	parse_func_map[std::string("ѹ��ƫ��")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		base->compdev = (float)atof(field);
	};
	parse_func_map[std::string("ѹ���ٷֱ�")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		base->compdevpercent = (float)atof(field);
	};
	parse_func_map[std::string("�ѹ�����")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		base->comptimemax = atoi(field);
	};
	parse_func_map[std::string("���ѹ�����")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		base->comptimemin = atoi(field);
	};
	parse_func_map[std::string("����ƫ��")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		base->excdev = (float)atof(field);
	};
	parse_func_map[std::string("����ٷֱ�")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		base->excdevpercent = (float)atof(field);
	};
	parse_func_map[std::string("�������")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		base->exctimemax = atoi(field);
	};
	parse_func_map[std::string("���������")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		base->exctimemin = atoi(field);
	};
	parse_func_map[std::string("���")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		base->classof = ConvertStringToEnum<GOLDEN_CLASSOF>(field);
	};
	parse_func_map[std::string("�޸�ʱ��")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
	}; 
	parse_func_map[std::string("�޸���")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
	}; 
	parse_func_map[std::string("����ʱ��")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
	}; 
	parse_func_map[std::string("������")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
	};
	parse_func_map[std::string("����")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		base->mirror = (golden_byte)atoi(field);
	};
	parse_func_map[std::string("ʱ�������")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		if (atoi(field) != 0) memcpy(&(base->millisecond), &g_on, 1);
	};
	parse_func_map[std::string("����ͳ��")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		if (atoi(field) != 0) memcpy(&(base->summary), &g_on, 1);
	};
	// ����Դ, �ɼ�, �豸��ǩ, LOCATION1, LOCATION2, LOCATION3, LOCATION4, LOCATION5, USERINT1, USERINT2, USERREAL1, USERREAL2
	parse_func_map[std::string("����Դ")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		memcpy(scan->source, field, GOLDEN_MIN(GOLDEN_SOURCE_SIZE, strlen(field)));
	};
	parse_func_map[std::string("�ɼ�")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		if (atoi(field) != 0) memcpy(&(scan->scan), &g_on, 1);
	};
	parse_func_map[std::string("�豸��ǩ")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
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
	// ����ʽ, ������ʽ, ʱ����ο�, ��������
	parse_func_map[std::string("����ʽ")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		memcpy(calc->equation, field, GOLDEN_MIN(GOLDEN_EQUATION_SIZE, strlen(field)));
	};
	parse_func_map[std::string("������ʽ")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		calc->trigger = (golden_byte)atoi(field);
	};
	parse_func_map[std::string("ʱ����ο�")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		calc->timecopy = (golden_byte)atoi(field);
	};
	parse_func_map[std::string("��������")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		calc->period = atoi(field);
	};
	// id���������һ�У�����IDΪ׼���±�ǩ�����ԣ���������
	parse_func_map[std::string("id")] = [](char *field, GOLDEN_POINT *base, GOLDEN_SCAN_POINT *scan, GOLDEN_CALC_POINT *calc) {
		base->id = atoi(field);
	};

	// ��ȡ���ݿ�Ԫ������Ϣ
	golden_error ecode = GoE_OK;
	int nHanle = 0;
	{
		// �����ӳػ�ȡ���Ӿ��
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

	// ��ȡ�����
	int tables_count = 0;
	ecode = gob_tables_count(nHanle, &tables_count);
	if (ecode != GoE_OK) {
		check_ecode(ecode, "Get tables count", g_logger);
		return false;
	}
	g_logger->info("Get [{}] tables in database", tables_count);

	if (0 == tables_count)
		return true;

	// ��ȡ��ID
	vector<int> tables_id(tables_count);
	ecode = gob_get_tables(nHanle, tables_id.data(), &tables_count);
	if (ecode != GoE_OK) {
		check_ecode(ecode, "Get tables id", g_logger);
		return false;
	}

	// ��ȡ���б���Ϣ
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

// �������
bool create_point() 
{
	auto create_point_func = [&](const std::string &point_file_path) {
		// ���ļ�
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
			// �����ӳػ�ȡ���Ӿ��
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

		// �����Ƿ���ڣ��������򴴽�
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

		// ���һ�еĸ����ֶε�ָ��
		std::vector<char*> fields_ptr;
		size_t field_index = 0;
		size_t id_field_number = 0;

		// ���ݱ����ж������˳��
		// ��ȡ������ÿ���ֶε�ָ��
		char* p = const_cast<char*>(title_line.c_str());
		char* p_end = p + title_line.size();
		do {
			fields_ptr.push_back(p);
			while (*p != '\r' && *p != '\n' && *p != ',') ++p;
			*p++ = '\0';
		}while(p < p_end);
		// ����ÿ���ֶεĽ�������
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
		
		// ��ǩ������
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

			// ��ʼ����ǩ������
			if (update_point_flow) {
				// ���Ƚ���id
				line_parse_func[id_field_number](fields_ptr[id_field_number], &base, &scan, &calc);
				// ��ȡ��ǩ������
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

			// ����������
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
				// �����ǩ���Ѿ����ڣ����������
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
			//if (base.id) {  // ָ��ID�����������
			//	ecode = gob_update_point_property(nHanle, &base, &scan, &calc);
			//	if (ecode != GoE_OK) {
			//		check_ecode(ecode, fmt::format("Update point {}", base.tag).c_str(), g_logger);
			//		return;
			//	}
			//	g_logger->trace("Update point [{}.{}] succesful.", table_name, base.tag);
			//}
			//else {
			//	ecode = gob_insert_point(nHanle, &base, &scan, &calc);
			//	// �����ǩ���Ѿ����ڣ����������
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
			// �����Ѿ�ͬ����ɵĵ��
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