#include <ctime>
#include <cstdio>
#include <iostream>
#include <vector>
#include <mutex>
#include <math.h>
#include <queue>
#include <exception>
#include <unordered_map>
#include <csignal>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_sinks.h"
#include "spdlog/sinks/null_sink.h"
#include "spdlog/details/file_helper.h"
#include "spdlog/details/os.h"

#include "CLI/CLI.hpp"
#include "goldenapi.h"
#include "common.h"
#include "scope_guard.h"
#include "thread_pool.h"

#ifdef _WIN32
#include <tchar.h>
#else
#define _T(x) x
#endif // _WIN32

using namespace std;
namespace spd = spdlog;
typedef std::shared_ptr<spdlog::logger> plog;

namespace golden_config
{
	std::string task_name_;
	std::string source_host_name_;
	int source_port_;
	std::string source_user_;
	std::string source_password_;
	std::string sink_host_name_;
	int sink_port_;
	std::string sink_user_;
	std::string sink_password_;
	std::string start_time_;
	std::string end_time_;
	int thread_count_;
	std::string points_dir_;
	bool output_points_prop_;
	int log_level_;
	bool print_log_;

	int start_time_int_;
	short start_time_ms_;
	int end_time_int_;
	short end_time_ms_;
};

#define VALUE_TYPE(type) (type >= GOLDEN_REAL16 && type <= GOLDEN_REAL64) ? 1 : ((type >= GOLDEN_BOOL && type <= GOLDEN_INT64) ? -1 : 0)
#define BATCH_COUNT 10000

bool g_running = false;
static thread_pool g_thread_pool;
golden_int32 source_point_count = 1000000;
std::vector<std::string> points_files;
unordered_map<golden_int32, int> source_id_to_value_type;  // 1浮点 -1整型 0不支持的类型
unordered_map<golden_int32, golden_int32> source_id_to_sink_id;
std::string g_app_root_path;

// 连接池
queue<golden_int32> source_db_connect_queue;
queue<golden_int32> sink_db_connect_queue;
std::mutex cq_mutex;

// 日志
std::shared_ptr<spdlog::logger> g_logger = nullptr;
std::shared_ptr<spdlog::logger> g_progress = nullptr;

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

// 从CSV文件中获取要同步的标签点
void get_points_from_file(const std::string &point_file_path, vector<golden_int32>& ids, vector<int>& types, vector<std::string>& fullnames)
{
	// 打开文件
	input_file ifile(point_file_path);
	auto& ifs = ifile.ifs();
	ifs.seekg(ifstream::beg);
	std::string line;
	getline(ifs, line);
	// 验证前两列的标题是否符合规范
	if (line.find(_T("id")) == std::string::npos || line.find(_T("table_dot_tag")) == std::string::npos)
	{
		g_logger->error(_T("point prop file must include field id and table_dot_tag."));
		return;
	}
	// 获取ID和名字
	size_t sep = 0, sep_p = 0, sep_n = 0;
	int id_count = 0;
	while (getline(ifs, line) && id_count++ < source_point_count)
	{
		sep = line.find_first_of(',');
		int&& id = atoi(line.substr(0, sep).c_str());
		ids.push_back(id);
		if (source_id_to_value_type.find(id) != source_id_to_value_type.end())
			types.push_back(source_id_to_value_type.at(id));
		else
			g_logger->error(_T("The information in the file dose not match the information in the database, id={}, line={}"), id, line);
		sep_p = line.find_first_of('[', sep + 1);
		sep_n = line.find_first_of(']', sep + 1);
		fullnames.push_back(line.substr(sep_p + 1, sep_n - sep_p - 1));
	}
}

// 将source数据库的历史数据同步到sink数据库的线程，只处理一个点表文件
void hisdata_to_rtdb_thread(const std::string &point_file_path)
{
	// 提取文件名
	int position = point_file_path.find_last_of(PATH_SEP) + 1;
	std::string data_file_name = point_file_path.substr(position, point_file_path.find_last_of('.') - position);
	g_logger->info(_T("Open [{}], parsing..."), data_file_name );
	golden_error ecode = GoE_OK;

	// 获取要处理点的ID、类型、名称
	vector<golden_int32> ids;
	vector<int> types;
	vector<std::string> fullnames;
	get_points_from_file(point_file_path, ids, types, fullnames);
	golden_int32 nPointCount = (golden_int32)ids.size();
	if (nPointCount == 0) return;

	// 获取要同步的时间范围
	golden_int32 nStartTime = golden_config::start_time_int_;
	golden_int32 nEndTime = golden_config::end_time_int_;
	std::string strStartTime = cast_time_str_simple(nStartTime);
	std::string strEndTime = cast_time_str_simple(nEndTime);
	g_logger->info(_T("Sync time range : from [{}] to [{}]"), golden_config::start_time_, golden_config::end_time_);

	// 并发线程数
	golden_int32 threadcount = golden_config::thread_count_;

	// 同步成功的历史值总数
	volatile golden_int32 total_values_count = 0;

	// 初始化线程池及工作函数
	std::mutex cq_mutex;
	g_thread_pool.init(threadcount);
	vector<std::future<golden_error> >results;
	auto sync_func = [&](golden_int32 pos)->golden_error{
		int source_db_handle = 0;
		int sink_db_handle = 0;
		int source_id = ids.at(pos);
		const char* point_name = fullnames.at(pos).c_str();
		if (source_id_to_sink_id.find(source_id) == source_id_to_sink_id.end()) {
			g_logger->info(_T("Point [{},{}] is not in [{}], skip it."), source_id, point_name, golden_config::sink_host_name_);
			return GoE_FALSE;
		}
		int sink_id = source_id_to_sink_id.find(source_id)->second;
		golden_error ecode = GoE_FALSE;
		if (pos >= nPointCount) return GoE_FALSE;

		// 从连接池获取连接
		{
			std::unique_lock<std::mutex> lock(cq_mutex);
			source_db_handle = source_db_connect_queue.front();
			source_db_connect_queue.pop();
			sink_db_handle = sink_db_connect_queue.front();
			sink_db_connect_queue.pop();
		}
		// 返回时自动归还连接
		scope_guard on_failure_rollback([&]
		{
			std::unique_lock<std::mutex> lock(cq_mutex);
			source_db_connect_queue.push(source_db_handle);
			sink_db_connect_queue.push(sink_db_handle);
		});
		
		// 获取source点的数据个数
		golden_int32 source_values_count = 0;
		ecode = goh_archived_values_count(source_db_handle, source_id, nStartTime, 0, nEndTime, 0, &source_values_count);
		if (source_values_count == 0) {
			g_logger->info(_T("Point [{},{}] history data is empty, sync completed."), source_id, point_name);
			return GoE_OK;
		}
		g_logger->info(_T("Point [{},{}][{}-{}] may has [{}] history records, prepare to query..."), source_id, point_name, strStartTime, strEndTime, source_values_count);
		CHECK_ECODE(ecode, _T("source:goh_archived_values_count"), g_logger);
		// 获取sink点的数据个数
		golden_int32 sink_values_count = 0;
		ecode = goh_archived_values_count(sink_db_handle, sink_id, nStartTime, 0, nEndTime, 0, &sink_values_count);
		CHECK_ECODE(ecode, _T("sink:goh_archived_values_count"), g_logger);
		if (source_values_count == sink_values_count) {
			g_logger->info(_T("Point [{},{}][{}-{}] has the same count in {} and {}, that is {}, sync completed."), source_id, point_name, strStartTime, strEndTime, golden_config::source_host_name_, golden_config::sink_host_name_, sink_values_count);
			return GoE_OK;
		}
		else
			g_logger->trace(_T("Point [{},{}][{}-{}] has {} in {}，has {} in {}, needs sync."), source_id, point_name, strStartTime, strEndTime, source_values_count, golden_config::source_host_name_, sink_values_count, golden_config::sink_host_name_);
		
		// 申请内存
		int batch_count = GOLDEN_MIN(source_values_count, BATCH_COUNT);
		if (batch_count == 1) batch_count = 2;    // 最小为2，可以放下开始、结束时间
		vector<golden_int32> put_ids(batch_count, sink_id);
		vector<golden_int64> states(batch_count);
		vector<golden_float64> values(batch_count);
		vector<golden_int32> datetimes(batch_count, 0);
		vector<golden_int16> mses(batch_count, 0);
		vector<golden_int16> qualities(batch_count, 0);
		vector<golden_error> errors(batch_count, 0);

		// 设置查询范围
		int batch_nStartTime = nStartTime;
		int batch_nEndTime = nEndTime;

		// 分批同步
		while (batch_nStartTime <= batch_nEndTime) {
			datetimes[0] = batch_nStartTime;
			datetimes[batch_count - 1] = batch_nEndTime;

			// 获取source历史值
			golden_int32 values_count_get = batch_count;
			ecode = goh_get_archived_values(source_db_handle, source_id, &values_count_get, datetimes.data(), mses.data(), values.data(), states.data(), qualities.data());
			CHECK_ECODE(ecode, _T("goh_get_archived_values"), g_logger);
			if (values_count_get == 0) {
				g_logger->info(_T("Point [{},{}][{}-{}] left 0 datas, skip it."), source_id, point_name, cast_time_str_simple(batch_nStartTime), cast_time_str_simple(batch_nEndTime));
				return GoE_OK;
			}
			else {
				g_logger->trace(_T("Point [{},{}][{}-{}] has [{}] datas, sync..."), source_id, point_name, cast_time_str_simple(datetimes[0]), cast_time_str_simple(datetimes[values_count_get - 1]), values_count_get);
			}
			// 更新开始时间
			batch_nStartTime = datetimes[values_count_get - 1] + 1;
			// 向sink写入历史值
			golden_int32 values_count_put = values_count_get;
			ecode = goh_put_archived_values(sink_db_handle, &values_count_put, put_ids.data(), datetimes.data(), mses.data(), values.data(), states.data(), qualities.data(), errors.data());
			g_logger->info(_T("Point [{},{}][{}-{}] syncs [{}] datas, sync completed."), source_id, point_name, cast_time_str_simple(datetimes[0]), cast_time_str_simple(datetimes[values_count_put - 1]), values_count_put);
			CHECK_ECODE(ecode, _T("goh_put_archived_values"), g_logger);
		
			// 查看写入失败的数据
			if (values_count_put != values_count_get) {
				int error_data_count = 0;
				for (int i = (int)errors.size() - 1; i >= 0; --i) {    // 错误一般在后面，从后往前遍历
					if (errors[i] != GoE_OK) {
						g_logger->error(_T("Point [{},{}] No.{} in datas sync failed."), source_id, point_name, i + 1);
						CHECK_ECODE(errors[i], _T("goh_put_archived_values"), g_logger);
						if (++error_data_count == (values_count_get - values_count_put)) break;  // 如果检查出的错误数已够，提前跳出循环
					}
				}
			}
			total_values_count += values_count_put;
		}

		// 将补历史缓存中的数据归档，以便查询
		int values_count_flush = 0;
		ecode = goh_flush_archived_values(sink_db_handle, put_ids.at(0), &values_count_flush);
		CHECK_ECODE(ecode, _T("goh_flush_archived_values"), g_logger);
		
		return ecode;
	};
	g_logger->info(_T("Synchronizing [{}] history data, please wait a moment."), data_file_name);
	int completed_count = 0;
	int show_process = (int)ceil((double)nPointCount / 100);
	if (show_process == 0) show_process = 1;
	// 耗时统计
	stop_watch watch;
	watch.start();
	// 任务添加到线程池
	for (int i = 0; i < nPointCount; ++i)
		results.emplace_back(g_thread_pool.enqueue(sync_func, i));
	// 等待结果，显示进度
	for (auto&& result : results)
	{
		golden_error ret = result.get();
		if (completed_count++ % show_process == 0)
		{
			g_logger->info(_T("Completed progress is : {}%"), completed_count * 100 / nPointCount);
		}
	}
	watch.stop();
	g_logger->info(_T("Completed progress is : 100%"));
	g_thread_pool.close();
	g_logger->info(_T("Query [{}] history data records count : {}, time span : {}s, total time elapsed : {}ms"), data_file_name, total_values_count, nEndTime - nStartTime + 1, watch.elapsed_ms());
	
	// 重命名已完成点表
	std::string old_file_path = golden_config::points_dir_ + PATH_SEP + data_file_name + ".csv";
	std::string new_file_path = golden_config::points_dir_ + PATH_SEP + "[OK]" + data_file_name + ".csv";
	ecode = rename(old_file_path.c_str(), new_file_path.c_str());
	if (ecode != GoE_OK) g_logger->error(_T("Rename {} to {} failed."), old_file_path, new_file_path);
}

golden_error get_points_id(golden_int32 handle, const char *table_name, golden_int32 *ids, golden_int32 *count)
{
	golden_error ecode = GoE_FALSE;
	ecode = gob_search(handle, "*", table_name, NULL, NULL, NULL, NULL, GOLDEN_SORT_BY_ID, ids, count);
	if (ecode != GoE_OK)
	{
		g_logger->error("search {} failed", table_name);
		CHECK_ECODE(ecode, _T("gob_search"), g_logger);
	}
	if (count == 0)
	{
		g_logger->error("search nothing.");
	}
	return ecode;
}

// 获取文件夹下所有标签点文件
size_t get_points_files(std::string points_dir)
{
	if (points_dir.length())
	{
#ifdef _WIN32
		WIN32_FIND_DATA fd;
		points_dir += (points_dir.rfind('\\') == 0) ? "" : "\\";
		std::string find_points_filter = points_dir + _T("*.csv");
		HANDLE hFind = FindFirstFile(find_points_filter.c_str(), &fd);
		if (hFind == INVALID_HANDLE_VALUE)
		{
			g_logger->error(_T("遍历标签点所在文件夹没有找到符合条件的文件"));
			return 0;
		}
		do
		{
			if (fd.cFileName[0] != _T('.'))
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

// 准备两个数据库的连接池
void prepare_connections()
{
	std::mutex login_mutex;
	golden_error ecode = GoE_OK;
	std::string source_ip = golden_config::source_host_name_;
	int source_port = golden_config::source_port_;
	std::string source_user = golden_config::source_user_;
	std::string source_password = golden_config::source_password_;
	std::string sink_ip = golden_config::sink_host_name_;
	int sink_port = golden_config::sink_port_;
	std::string sink_user = golden_config::sink_user_;
	std::string sink_password = golden_config::sink_password_;
	golden_int32 threadcount = golden_config::thread_count_;

	go_set_option(GOLDEN_API_AUTO_RECONN, 1);
	go_set_option(GOLDEN_API_CONN_TIMEOUT, 0);

	auto func_connect = [&](std::string ip, int port, std::string user, std::string password)->golden_int32
	{
		bool login_success = false;
		int nHandle = 0, priv = 0;
		while (login_success == false)
		{
			{
				std::unique_lock<std::mutex> lock(login_mutex);
				login_success = (GoE_OK == (ecode = go_connect(ip.c_str(), port, &nHandle)) && GoE_OK == (ecode = go_login(nHandle, user.c_str(), password.c_str(), &priv)));
			}
			if (!login_success)
			{
				this_thread::sleep_for(std::chrono::seconds(3));
				nHandle = 0;
				login_success = false;
				CHECK_ECODE(ecode, "Connect golden rtdb", g_logger);
			}
		}
		g_logger->trace("Connect golden rtdb host name : {}.", ip);
		return nHandle;
	};

	// 不能大于单个客户端允许最大连接数
	golden_uint32 conntion_count_per_client = 0;
	int db_handle = func_connect(source_ip, source_port, source_user, source_password);
	if (ecode != GoE_OK) return;
	ecode = go_get_db_info2(db_handle, GOLDEN_PARAM_ONE_CLINET_MAX_CONNECTION_COUNT, &conntion_count_per_client);
	CHECK_ECODE(ecode, "Get one client max connection count", g_logger);
	if (ecode != GoE_OK) return;
	if (golden_config::thread_count_ >= (int)conntion_count_per_client)
	{
		g_logger->warn("The number of threads is too large and the number of connections required exceeds the maximum number of connections allowed by a single client, and the maximum can be set to {}.", conntion_count_per_client - 1);
		golden_config::thread_count_ = conntion_count_per_client - 1;
	}
	source_db_connect_queue.push(db_handle);

	for (int i = 0; i < threadcount; ++i)
	{
		source_db_connect_queue.push(func_connect(source_ip, source_port, source_user, source_password));
		sink_db_connect_queue.push(func_connect(sink_ip, sink_port, sink_user, sink_password));
	}
	g_logger->trace("Source rtdb connection count is {}.", source_db_connect_queue.size());
	g_logger->trace("Sink rtdb connection count is {}.", sink_db_connect_queue.size());
}

// 释放连接
void release_conncetions()
{
	while (!source_db_connect_queue.empty())
	{
		go_disconnect(source_db_connect_queue.front());
		source_db_connect_queue.pop();
	}
	while (!sink_db_connect_queue.empty()) {
		go_disconnect(sink_db_connect_queue.front());
		sink_db_connect_queue.pop();
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
		auto log_file_ = log_root_path + "/" + golden_config::task_name_ + "_logger.csv";
		fmt::print("Open log file {}\n", log_file_);
		// 输出到文件
		auto file_logger_ = std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_file_, true);
		assert(file_logger_);
		auto stdout_logger_ = std::make_shared<spdlog::sinks::stdout_sink_mt>();
		assert(stdout_logger_);

		// 打印进度
		spdlog::sinks_init_list logger_list = { file_logger_, stdout_logger_ };
		g_progress = std::make_shared<spdlog::logger>(golden_config::task_name_, std::move(logger_list));
		g_progress->set_pattern("%^%t,progress,\"%v\"%$");
		g_progress->set_level(spdlog::level::level_enum(golden_config::log_level_));

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
		golden_error ecode = GoE_OK;
		// start_time
		if (!golden_config::start_time_.empty()) {
			if (0 == golden_config::start_time_.compare("now")) {
				golden_config::start_time_int_ = (int)time(0);
				golden_config::start_time_ms_ = 0;
			}
			else if (0 == golden_config::start_time_.compare("mintime")) {
				golden_config::start_time_int_ = 0;
				golden_config::start_time_ms_ = 0;
			}
			else {
				golden_int64 _start_time = -1;
				golden_int16 _start_time_ms = 0;
				auto pot_pos = golden_config::start_time_.find('.', 0);
				if (pot_pos != std::string::npos) {
					char *p_s = (char*)golden_config::start_time_.data();
					*(p_s + pot_pos) = '\0';
					ecode = go_parse_time(p_s, &_start_time, &_start_time_ms);
					if (ecode != GoE_OK) throw std::invalid_argument(fmt::format("Parse start time failed : {}", golden_config::start_time_).c_str());
					*(p_s + pot_pos) = '.';
					char* p_ms = p_s + pot_pos + 1;
					_start_time_ms = (short)atoi(p_ms);
				}
				else {
					ecode = go_parse_time(golden_config::start_time_.c_str(), &_start_time, &_start_time_ms);
					if (ecode != GoE_OK) throw std::invalid_argument(fmt::format("Parse start time failed : {}", golden_config::start_time_).c_str());
				}
				if (_start_time >= 0) {
					golden_config::start_time_int_ = (int)_start_time;
					golden_config::start_time_ms_ = _start_time_ms;
				}
				else {
					throw std::invalid_argument(fmt::format("Parse start time failed : {}", golden_config::start_time_).c_str());
				}
			}
		}
		else {
			throw std::invalid_argument("start time is empty.");
		}

		// end_time
		if (!golden_config::end_time_.empty()) {
			if (0 == golden_config::end_time_.compare("forever")) {
				golden_config::end_time_int_ = INT32_MAX;
				golden_config::end_time_ms_ = 999;
			}
			else {
				golden_int64 _end_time = -1;
				golden_int16 _end_time_ms = 0;
				auto pot_pos = golden_config::end_time_.find('.', 0);
				if (pot_pos != std::string::npos) {
					char *p_s = (char*)golden_config::end_time_.data();
					*(p_s + pot_pos) = '\0';
					ecode = go_parse_time(p_s, &_end_time, &_end_time_ms);
					if (ecode != GoE_OK) throw std::invalid_argument(fmt::format("Parse end time failed : {}", golden_config::end_time_).c_str());
					*(p_s + pot_pos) = '.';
					char* p_ms = p_s + pot_pos + 1;
					_end_time_ms = (short)atoi(p_ms);
				}
				else {
					ecode = go_parse_time(golden_config::end_time_.c_str(), &_end_time, &_end_time_ms);
					if (ecode != GoE_OK) throw std::invalid_argument(fmt::format("Parse end time failed : {}", golden_config::end_time_).c_str());
				}
				if (_end_time >= 0) {
					golden_config::end_time_int_ = (int)_end_time;
					golden_config::end_time_ms_ = _end_time_ms;
				}
				else {
					throw std::invalid_argument(fmt::format("Parse end time failed : {}", golden_config::end_time_).c_str());
				}
			}
		}
		else {
			throw std::invalid_argument("end time is empty.");
		}

		if (GOLDEN_TIME_GREATER_THAN(golden_config::start_time_int_, golden_config::start_time_ms_, golden_config::end_time_int_, golden_config::end_time_ms_)) {
			throw std::invalid_argument("start time can not greater than end time.");
		}

		// thread count
		if (golden_config::thread_count_ < 1) {
			throw std::invalid_argument("thread count must >= 1.");
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
	g_logger->trace("{}:begin.", __FUNCTION__);

	//数据库通信句柄
	int source_db_handle = 0;
	int sink_db_handle = 0;

	golden_error ecode = GoE_OK;
	{
		// 从连接池获取连接句柄
		std::unique_lock<std::mutex> lock(cq_mutex);
		g_logger->trace("Get connection handle");
		source_db_handle = source_db_connect_queue.front();
		source_db_connect_queue.pop();
		sink_db_handle = sink_db_connect_queue.front();
		sink_db_connect_queue.pop();
	}
	ON_SCOPE_EXIT([&] {
		if (source_db_handle) {
			std::unique_lock<std::mutex> lock(cq_mutex);
			source_db_connect_queue.push(source_db_handle);
			sink_db_connect_queue.push(sink_db_handle);
			g_logger->trace("Return connection handle : source={}, sink={}", source_db_handle, sink_db_handle);
		}
	});

	//获取授权中标签点容量
	golden_uint32 lic_point_count = 0;
	go_get_db_info2(source_db_handle, GOLDEN_PARAM_LIC_TAGS_COUNT, &lic_point_count);
	source_point_count = (golden_int32)lic_point_count;

	//获取所有标签点ID
	vector<golden_int32> source_all_ids(source_point_count, 0);
	ecode = get_points_id(source_db_handle, nullptr, source_all_ids.data(), &source_point_count);
	CHECK_ECODE(ecode, "get_points_id", g_logger);
	if (GoE_OK != ecode)		return false;
	source_all_ids.resize(source_point_count);
	source_id_to_value_type.reserve(source_point_count);
	source_id_to_sink_id.reserve(source_point_count);

	std::string output_file_name = golden_config::points_dir_ + "/all_points.csv";
	output_file ofile(output_file_name, golden_config::output_points_prop_);
	auto& ofs = ofile.ofs();
	if (golden_config::output_points_prop_) ofs << "id,table_dot_tag,desc,classof" << endl;

	int batch = 1000;
	vector<GOLDEN_POINT> source_points_prop(batch);
	vector<golden_error> gerror(batch, 0);
	vector<char*> points_name(batch, 0);
	vector<int> temp_ids(batch, 0);
	auto func_get_prop = [&](vector<int>& points) {
		int points_count = points.size();
		for (int i = 0; i < points_count; ++i) {
			source_points_prop[i].id = points[i];
		}
		ecode = gob_get_points_property(source_db_handle, points_count, source_points_prop.data(), nullptr, nullptr, gerror.data());
		for (int i = 0; i < points_count; ++i) {
			if (GoE_OK == gerror[i]) {
				source_id_to_value_type.insert(make_pair(source_points_prop[i].id, VALUE_TYPE(source_points_prop[i].type)));
				if (golden_config::output_points_prop_) {
					if (source_points_prop[i].type >= GOLDEN_BOOL && source_points_prop[i].type <= GOLDEN_REAL64) {
						ofs << dec << source_points_prop[i].id << ",[" << source_points_prop[i].table_dot_tag << "]," << source_points_prop[i].desc << "," << trans_classof(source_points_prop[i].classof) << endl;
					}
					else {
						g_logger->warn("id={}, name={}, type={}, value type is not in [BOOL,CHAR,INT,FLOAT]", source_points_prop[i].id, source_points_prop[i].table_dot_tag, source_points_prop[i].type);
					}
				}
			}
			else {
				CHECK_ECODE(gerror[i], fmt::format("gob_get_points_property error : id={}", source_points_prop[i].id).c_str(), g_logger);
			}
			points_name[i] = source_points_prop[i].table_dot_tag;
		}
		int find_points_count = points_count;
		ecode = gob_find_points(sink_db_handle, &find_points_count, points_name.data(), temp_ids.data(), nullptr, nullptr, nullptr);
		for (int i = 0; i < points_count; ++i) {
			if (temp_ids[i] == 0)
				g_logger->error(_T("Not find {} in {}"), points_name[i], golden_config::sink_host_name_);
			else
				source_id_to_sink_id.insert(make_pair(source_points_prop[i].id, temp_ids[i]));
		}
	};

	vector<int> points_to_find;
	points_to_find.reserve(batch);
	for (auto id : source_all_ids)
	{
		points_to_find.push_back(id);
		if (points_to_find.size() == batch)
		{
			func_get_prop(points_to_find);
			points_to_find.clear();
			points_to_find.reserve(batch);
		}
	}
	if (points_to_find.size() > 0) {
		func_get_prop(points_to_find);
		points_to_find.clear();
	}

	if (golden_config::output_points_prop_) {
		ofs.flush();
		g_logger->info("Output all points info compeleted.（BOOL,CHAR,INT,FLOAT）");
		g_logger->info("See details in [{}]", output_file_name);
	}

	source_points_prop.clear();
	return true;
}

// 同步数据
bool sync_data() {
	size_t points_file_count = get_points_files(golden_config::points_dir_);
	if (points_file_count > 0) {
		g_logger->info(_T("There are {} tasks to deal with."), points_file_count);
		std::thread thd;
		stop_watch watch;
		watch.start();
		for (size_t i = 0; i < (size_t)points_file_count; ++i) {
			// 跳过已经同步完成的点表
			if (points_files.at(i).find("[OK]") == std::string::npos) {
				thd = std::thread(hisdata_to_rtdb_thread, points_files.at(i));
				thd.join();
			}
			else {
				g_logger->info(_T("[{}] history data synchronized."), points_files.at(i));
			}
			g_progress->critical(_T("Task {} completed, progress is : {}%"), i + 1, (i + 1) * 100 / points_file_count);
		}
		watch.stop();
		g_progress->critical(_T("There are {} files synchronized, total elapsed is {}s"), points_file_count, watch.elapsed_second());
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

int main(int argc, char* argv[])
{
	std::cout << ",-_/,.       .                 .-,--.      .        .---.             " << endl;
	std::cout << "' |_|/ . ,-. |- ,-. ,-. . .    ' |   \\ ,-. |- ,-.   \\___  . . ,-. ,-. " << endl;
	std::cout << " /| |  | `-. |  | | |   | |    , |   / ,-| |  ,-|       \\ | | | | |   " << endl;
	std::cout << " `' `' ' `-' `' `-' '   `-|    `-^--'  `-^ `' `-^   `---' `-| ' ' `-' " << endl;
	std::cout << "                         /|                                /|         " << endl;
	std::cout << "                        `-'                               `-'         " << endl << endl;
	// http://patorjk.com/software/taag/#p=display&f=Stampatello&t=History%20Data%20Sync

	signal(SIGINT, sig_handler);

	CLI::App app{ "App description" };
	{
		golden_config::task_name_ = "main";
		app.add_option("-n,--taskname", golden_config::task_name_, "task name (=main)")->required();
		golden_config::source_host_name_ = "127.0.0.1";
		app.add_option("--source_host_name", golden_config::source_host_name_, "source host name (=127.0.0.1)")->required();
		golden_config::source_port_ = 6327;
		app.add_option("--source_port", golden_config::source_port_, "source port number (=6327)");
		golden_config::source_user_ = "sa";
		app.add_option("--source_user", golden_config::source_user_, "source user name (=sa)");
		golden_config::source_password_ = "admin";
		app.add_option("--source_password", golden_config::source_password_, "source pass word (=admin)");
		golden_config::sink_host_name_ = "127.0.0.1";
		app.add_option("--sink_host_name", golden_config::sink_host_name_, "sink host name (=127.0.0.1)")->required();
		golden_config::sink_port_ = 6327;
		app.add_option("--sink_port", golden_config::sink_port_, "sink port number (=6327)");
		golden_config::sink_user_ = "sa";
		app.add_option("--sink_user", golden_config::sink_user_, "sink user name (=sa)");
		golden_config::sink_password_ = "admin";
		app.add_option("--sink_password", golden_config::sink_password_, "sink pass word (=admin)");
		golden_config::start_time_ = "now";
		app.add_option("-s,--start_time", golden_config::start_time_, "start time (=now)\nformat:\n \"YYYY-MM-DD hh:mm:ss.ms\"\n \"mintime\" start time is min UTC time : 1970-01-01 00:00:00.000\n \"now\" start time is local real time\n Enclosed in single or double quotes.");
		golden_config::end_time_ = "forever";
		app.add_option("-e,--end_time", golden_config::end_time_, "end time (=forever)\nformat:\n \"YYYY-MM-DD hh:mm:ss.ms\"\n \"forever\" end time is max UTC time");
		golden_config::thread_count_ = 1;
		app.add_option("--thread_count", golden_config::thread_count_, "thread count (=1)");
		app.add_option("--points_dir", golden_config::points_dir_, "point file directory (*.csv)")->required();
		golden_config::output_points_prop_ = false;
		app.add_flag("--output_points_prop", golden_config::output_points_prop_, "out put all points' property");
		golden_config::print_log_ = true;
		app.add_flag("--print_log", golden_config::print_log_, "print log to console");
		golden_config::log_level_ = spdlog::level::level_enum::info;
		app.add_option("--log_level", golden_config::log_level_, "log level (=2) as info\n 0.trace\n 1.debug\n 2.info\n 3.warn\n 4.err\n 5.critical\n 6.off");
#ifdef _WIN32
		app.add_flag("--Attention", "# 注意：\n#   1.可以传入带毫秒的时间范围\n#   2.如果同步多个标签点，会自动分配到多个线程\n#   3.同步过的标签点文件会重命名添加[OK]前缀");
#elif _LINUX
		app.add_flag("--Attention", "Attention:\n1.Time range with milliseconds can be passed in.\n2.If multiple points are sync, they are automatically assigned to multiple sync threads.\n3.The synchronized point files will be renamed and prefixed with [OK].");
#endif

		try {
			(app).parse((argc), (argv));
		}
		catch (const CLI::ParseError &e) {
			return (app).exit(e);
		}
	}

	// 准备日志
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

	g_logger->trace("Querying data.");
	if (!sync_data())
		return -1;

	g_logger->info("Exit the app.");
	return 0;
}

