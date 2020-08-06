#include <cstdio>
#include <iostream>
#include <vector>
#include <mutex>
#include <math.h>
#include <queue>

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
	//外部传入参数
	std::string task_name_;
	std::vector<std::string> host_name_;
	int port_;
	std::string user_;
	std::string password_;
	std::string start_time_;
	std::string end_time_;
	int elapse_time_;
	int increment_time_;
	bool put_history_data_;
	int min_value_;
	int max_value_;
	std::string generator_;
	std::string search_condition_;
	int first_point_;
	int point_count_;
	int point_interval_;
	std::string write_mode_;
	int threadcount_;
	bool print_log_;
	int log_level_;
	int func_period_;
	std::string result_file_;

	//内部使用参数
	int start_time_int_;
	int end_time_int_;
};

enum data_type_e
{
	no_support = 0,
	analog = 1,
	digital = 2,
	boolean = 3
};

// 连接池
queue<golden_int32> connect_queue;
std::mutex cq_mutex;

// 日志
std::shared_ptr<spdlog::logger> g_logger = nullptr;
std::shared_ptr<spdlog::logger> g_progress = nullptr;

std::string g_app_root_path;
std::string app_name = "make_data";
bool g_running = false;
static thread_pool g_thread_pool;
double all_thread_all_call_total_elapsed = 0.0;    // 所有线程所有调用的总耗时
double all_thread_single_call_avg_elapsed = 0.0;   // 所有线程单次调用平均耗时
unsigned long long g_make_total_count = 0;         // 写入的总数据量
int period_count = 100;

// 数据库
std::vector<int> g_ids;
std::vector<int> g_types;
std::vector<data_type_e> g_types_simple;
std::vector<golden_int32> g_snapshot_time;

// 数据
// write mode = point
vector<golden_float64> g_vfvalues;   // 单点多时的浮点值
vector<golden_int64> g_vivalues;     // 单点多时的整数值
vector<golden_int64> g_vbvalues;     // 单点多时的BOOL值
// write mode = time
vector<vector<golden_float64>> g_vvfvalues;   // 浮点数的值
vector<vector<golden_int64>> g_vvxvalues;     // 整数/BOOL值混合

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

// 判断数据类型
data_type_e judge_data_type(golden_int32 data_type) 
{
	switch (data_type) {
	case GOLDEN_BOOL:
		return data_type_e::boolean;
	case GOLDEN_UINT8:
	case GOLDEN_INT8:
	case GOLDEN_CHAR:
	case GOLDEN_UINT16:
	case GOLDEN_INT16:
	case GOLDEN_UINT32:
	case GOLDEN_INT32:
	case GOLDEN_INT64:
		return data_type_e::digital;
	case GOLDEN_REAL16:
	case GOLDEN_REAL32:
	case GOLDEN_REAL64:
		return data_type_e::analog;
	default:
		return data_type_e::no_support;
	}
}

// 准备连接池
void prepare_connections()
{
	golden_error ecode = GoE_OK;
	
	go_set_option(GOLDEN_API_AUTO_RECONN, 1);
	go_set_option(GOLDEN_API_CONN_TIMEOUT, 0);

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
	if (golden_config::threadcount_ >= (int)conntion_count_per_client)
	{
		g_logger->warn("The number of threads is too large and the number of connections required exceeds the maximum number of connections allowed by a single client, and the maximum can be set to {}.", conntion_count_per_client - 1);
		golden_config::threadcount_ = conntion_count_per_client - 1;
	}
	connect_queue.push(db_handle);
	int host_count = (int)golden_config::host_name_.size();
	for (int i = 1; i < golden_config::threadcount_; ++i) 
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
		// start_time
		if (!golden_config::start_time_.empty()) {
			if (0 == golden_config::start_time_.compare("now")) {
				golden_config::start_time_int_ = (int)time(0);
			}
			else if (0 == golden_config::start_time_.compare("maxtime")) {
				golden_config::start_time_int_ = -1;
			}
			else {
				golden_int64 _start_time = -1;
				go_parse_time(golden_config::start_time_.c_str(), &_start_time, 0);
				if (_start_time >= 0) {
					golden_config::start_time_int_ = (int)_start_time;
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
			}
			else {
				golden_int64 _end_time = -1;
				go_parse_time(golden_config::end_time_.c_str(), &_end_time, 0);
				if (_end_time >= 0) {
					golden_config::end_time_int_ = (int)_end_time;
				}
				else {
					throw std::invalid_argument(fmt::format("Parse end time failed : {}", golden_config::end_time_).c_str());
				}
			}
		}
		else {
			throw std::invalid_argument("end time is empty.");
		}

		// thread count
		if (golden_config::threadcount_ < 1) {
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
	// 根据条件初始化ID数组，从first_point_，按照point_interval_等间隔递增
	g_logger->trace("{}:begin.", __FUNCTION__);
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

#ifdef _WIN32
	if (!golden_config::search_condition_.empty()) {
		int ids_count = 0;
		ecode = gob_search_point_id_count(nHanle, golden_config::search_condition_.c_str(), golden_config::search_condition_.size(), &ids_count);
		if (ecode != GoE_OK) {
			CHECK_ECODE(ecode, "Search point count", g_logger);
			return false;
		}
		if (ids_count == 0) {
			g_logger->warn("No point was found to satisfy the conditions.");
			return false;
		}
		golden_config::point_count_ = ids_count;
	}
#endif //_WIN32

	g_ids.resize(golden_config::point_count_);
	g_types.resize(golden_config::point_count_);
	g_types_simple.resize(golden_config::point_count_);
	g_snapshot_time.resize(golden_config::point_count_);

	g_logger->trace("{}:get ids.", __FUNCTION__);
#ifdef _WIN32
	if (!golden_config::search_condition_.empty()) {
		int ids_count = golden_config::point_count_;
		ecode = gob_search_point_ids(nHanle, golden_config::search_condition_.c_str(), golden_config::search_condition_.size(), g_ids.data(), &ids_count);
		if (ecode != GoE_OK) {
			CHECK_ECODE(ecode, "Search point by condition.", g_logger);
			return false;
		}
	}
	else {
		std::generate(g_ids.begin(), g_ids.end(), [n = 0]() mutable { return golden_config::first_point_ + (golden_config::point_interval_ * n++); });
}
#else  //_LINUX
	std::generate(g_ids.begin(), g_ids.end(), [n = 0]() mutable { return golden_config::first_point_ + (golden_config::point_interval_ * n++); });
#endif //_WIN32

	vector<golden_float64> snapshot_fvalue(golden_config::point_count_);
	vector<golden_int64> snapshot_ivalue(golden_config::point_count_);
	vector<golden_int16> snapshot_ms(golden_config::point_count_);
	vector<golden_int16> quality(golden_config::point_count_);
	vector<golden_error> errors(golden_config::point_count_);
	int point_count_tmp = golden_config::point_count_;
	g_logger->trace("{}:alloc snapshot mem.", __FUNCTION__);

	// 获取标签点数据类型
	ecode = gob_get_types_prpperty(nHanle, &point_count_tmp, g_ids.data(), g_types.data(), errors.data());
	if (ecode != GoE_OK) {
		CHECK_ECODE(ecode, "Get type property", g_logger);
		return false;
	}
	// 标签点类型转换
	std::generate(g_types_simple.begin(), g_types_simple.end(), [n = 0]() mutable { return judge_data_type(g_types[n++]); });
	// 获取标签点快照
	ecode = gos_get_snapshots(nHanle, &point_count_tmp, g_ids.data(), g_snapshot_time.data(), snapshot_ms.data(), snapshot_fvalue.data(), snapshot_ivalue.data(), quality.data(), errors.data());
	if (ecode != GoE_OK) {
		CHECK_ECODE(ecode, "Get snapshot values", g_logger);
		if (ecode == GoE_FALSE) {
			for (size_t i = 0; i < errors.size(); ++i) {
				CHECK_ECODE(errors[i], fmt::format("Get snapshot errors[{}], id={}", i, g_ids[i]).c_str(), g_logger);
			}
		}
		return false;
	}
	g_logger->trace("{}:get snapshot values.", __FUNCTION__);

	double coefficient = (6.28 / golden_config::func_period_);
	double A = (double)(golden_config::max_value_ - golden_config::min_value_) / 2;    //sin振幅
	double B = (double)(golden_config::max_value_ + golden_config::min_value_) / 2;    //sin偏移
	double K = (double)(golden_config::max_value_ - golden_config::min_value_) / golden_config::func_period_;  //line斜率
	int R = golden_config::max_value_ - golden_config::min_value_;    //rand振幅
	auto func_sin = [&]()->double {
		static int sin_i = 0;
		return (A * sin(coefficient * (sin_i++)) + B + ((double)rand() / RAND_MAX));  //增加0.0~1.0之间的随机数，模拟微小波动
	};
	auto func_line = [&]()->double {
		static int line_i = 0;
		return (K * line_i++ / golden_config::func_period_ + golden_config::min_value_ + ((double)rand() / RAND_MAX)); //增加0.0~1.0之间的随机数，模拟微小波动
	};
	auto func_rand = [&]()->double {
		return ((double)rand() / RAND_MAX * R + golden_config::min_value_);
	};

	g_logger->trace("{}:begin generate base data.", __FUNCTION__);
	if (0 == golden_config::write_mode_.compare("time")) {
		// 针对 write mode = time
		// 以时间断面写入数据，需要造一个周期的断面数据，也就是一个二维数组

		// 用g_vfvalues造一维基础数据
		g_vfvalues.resize(golden_config::func_period_);
		if (0 == golden_config::generator_.compare("sin")) {
			std::generate(g_vfvalues.begin(), g_vfvalues.end(), func_sin);
		}
		else if (0 == golden_config::generator_.compare("line")) {
			std::generate(g_vfvalues.begin(), g_vfvalues.end(), func_line);
		}
		else if (0 == golden_config::generator_.compare("rand")) {
			std::generate(g_vfvalues.begin(), g_vfvalues.end(), func_rand);
		}
		else {
			// 实现从文件获取数据
		}

		// 造一个周期的批次，每批次一个时刻断面数据
		g_vvfvalues.resize(golden_config::func_period_);
		g_vvxvalues.resize(golden_config::func_period_);

		for (int i = 0; i < golden_config::func_period_; ++i) {
			auto& current_fvalues = g_vvfvalues[i];
			auto& current_xvalues = g_vvxvalues[i];
			current_fvalues.resize(golden_config::point_count_);
			current_xvalues.resize(golden_config::point_count_);
			auto fv = g_vfvalues[i];
			std::generate(current_fvalues.begin(), current_fvalues.end(), [n = 0, fv]() mutable {
				return fv + ((double)rand() / RAND_MAX); });
			auto xv = static_cast<golden_int64>(g_vfvalues[i]);
			std::generate(current_xvalues.begin(), current_xvalues.end(), [n = 0, xv]() mutable { 
				return (g_types_simple[n++] == data_type_e::boolean) ? ((abs(xv) + 1) % 2) : xv; });
		}

		// 输出值
		//for (int i = 0; i < FUNC_PERIOD; ++i)
		//{
		//	for (int j = 0; j < golden_config::point_count_; ++j)
		//	{
		//		cout << "#1:" << i << "\t\t#2:" << j << "\t\tfvalue:" << g_vvfvalues[i][j] << "\t\txvalue:" << g_vvxvalues[i][j] << endl;
		//	}
		//}
	}
	else {
		// 针对 write mode = point
        // 多造一个周期的基础数据，每个点每次入库 FUNC_PERIOD * PERIOD_COUNT 条数据，
        // 为了防止出现所有点都从 pos=0 的位置开始，给每个点指定一个 0~FUNC_PERIOD 的随机起始位置，
        // 进行循环读取，达到一定程度的随机生成
		int base_value_count = golden_config::func_period_ * (period_count + 1);

		g_vfvalues.resize(base_value_count);
		g_vivalues.resize(base_value_count);
		g_vbvalues.resize(base_value_count);

		if (0 == golden_config::generator_.compare("sin")) {
			std::generate(g_vfvalues.begin(), g_vfvalues.end(), func_sin);
		}
		else if (0 == golden_config::generator_.compare("line")) {
			std::generate(g_vfvalues.begin(), g_vfvalues.end(), func_line);
		}
		else if (0 == golden_config::generator_.compare("rand")) {
			std::generate(g_vfvalues.begin(), g_vfvalues.end(), func_rand);
		}
		else {
			// 实现从文件获取数据
		}
		std::generate(g_vivalues.begin(), g_vivalues.end(), [n = 0]() mutable { return static_cast<golden_int64>(g_vfvalues[n++]); });
		std::generate(g_vbvalues.begin(), g_vbvalues.end(), [n = 0]() mutable { return (abs(g_vivalues[n++]) + 1) % 2; });
		// 输出值
		//for (int i = 0; i < base_value_count; ++i)
		//{
		//	cout << "#:" << i << "\t\tfvalue:" << g_vfvalues[i] << "\t\tivalue:" << g_vivalues[i] << "\t\tbvalue:" << g_vbvalues[i] << endl;
		//}
	}
	g_logger->trace("{}:end generate base data.", __FUNCTION__);

	return true;
}

golden_error put_same_point_thread(size_t pos)
{
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

	auto& point_id = g_ids[pos];
	auto point_type = judge_data_type(g_types[pos]);
	if (point_type == data_type_e::no_support) {
		g_logger->warn("No support data type, id={}", point_id);
		return GoE_OK;
	}
	auto& start_t_s = golden_config::start_time_int_;
	start_t_s = (start_t_s == -1) ? (g_snapshot_time[pos] + 1) : start_t_s;
	auto& end_t_s = golden_config::end_time_int_;
	auto& increment_t_ms = golden_config::increment_time_;
	auto increment_t_s = increment_t_ms / 1000;
	bool timestamp_is_second = (increment_t_ms % 1000 == 0) ? true : false;

	//填充数据
	const int batch_count = golden_config::func_period_ * period_count;
	vector<golden_int32> ids(batch_count, point_id);  // 填充id
	vector<golden_int32> datetimes(batch_count);
	vector<golden_int16> ms(batch_count, 0);
	vector<golden_int16> quality(batch_count, GOLDEN_Q_GOOD);
	vector<golden_error> errors(batch_count, 0);
	//值，换别名，针对不同类型选择不同的基础数据
	auto&& fvalues = g_vfvalues;
	auto&& ivalues = point_type == data_type_e::boolean ? g_vbvalues : g_vivalues;
	//函数封装，决定写快照还是写历史
	std::function<golden_error(golden_int32, golden_int32*, const golden_int32 *, const golden_int32 *, const golden_int16 *, const golden_float64 *, const golden_int64 *, const golden_int16 *, golden_error *)> put_datas = golden_config::put_history_data_ ? goh_put_archived_values : gos_put_snapshots;

	golden_int64 lasttime_ms = (golden_int64)start_t_s * 1000;
	golden_int64 endtime_ms = (golden_int64)end_t_s * 1000 + 999;
	stop_watch api_watch;
	auto sleep_until_time = std::chrono::milliseconds(golden_config::elapse_time_);
	auto sleep_time_if_not_ready = std::chrono::seconds(5);
	double elapsed_time = 0.0;
	unsigned long long total_count = 0;
	int base_values_start_pos = (int)pos % golden_config::func_period_;  //不同点开始位置
	while (lasttime_ms < endtime_ms)
	{
		auto&& abs_time = std::chrono::system_clock::now();
		for (int n = 0; n < batch_count; ++n)
		{
			if (timestamp_is_second) {
				auto&& _t = (int)(lasttime_ms / 1000) + n * increment_t_s;
				datetimes[n] = _t;
			}
			else {
				auto&& _t = lasttime_ms + n * increment_t_ms;
				datetimes[n] = (golden_int32)(_t / 1000);
				ms[n] = (golden_int16)(_t % 1000);
			}
			errors[0] = 0;
		}
		golden_int32 put_value_count = min(batch_count, (int)((endtime_ms - lasttime_ms) / increment_t_ms + 1));
		PUT_DATAS:
		auto put_value_count_successful = put_value_count;
		api_watch.restart();
		ecode = put_datas(nHanle, &put_value_count_successful, ids.data(), datetimes.data(), ms.data(),
			fvalues.data() + base_values_start_pos, ivalues.data() + base_values_start_pos, quality.data(), errors.data());
		if (ecode == GoE_ARV_PAGE_NOT_READY || ecode == GoE_ARVEX_PAGE_NOT_READY) {
			g_logger->warn("The database cache is full, wait 5 seconds to try again.");
			this_thread::sleep_for(sleep_time_if_not_ready);
			goto PUT_DATAS;
		}
		api_watch.stop();
		g_logger->info("Put datas batch: id={}, total={}, successful={}, lasttime={}, ret=0x{:x}, elapsed={}ms", 
			point_id, put_value_count, put_value_count_successful, lasttime_ms, ecode, api_watch.elapsed_ms());
		CHECK_ECODE(ecode, fmt::format("Put datas, id={}", point_id).c_str(), g_logger);
		lasttime_ms += (put_value_count * increment_t_ms);
		elapsed_time += api_watch.elapsed_ms();
		total_count += put_value_count;
		abs_time += sleep_until_time;
		base_values_start_pos = (base_values_start_pos + 1) % golden_config::func_period_;
		if (abs_time > std::chrono::system_clock::now()) {
			this_thread::sleep_until(abs_time);
		}
		if (ecode != GoE_OK) {
			return ecode;
		}
	}
	g_logger->warn("Put datas summary : id={}, total={}, elapsed={}ms, count_per_sec={}",
		point_id, total_count, elapsed_time, (double)(total_count * 1000) / elapsed_time);
	all_thread_all_call_total_elapsed += elapsed_time;
	g_make_total_count += total_count;

	//将所有标签点未写满的补历史缓存页写入存档文件中
	//如果需要立即查看数据，可以在程序退出时调用该函数，
	//运行中频繁调用会降低数据库性能
	int flush_count = 0;
	if (golden_config::put_history_data_) {
		ecode = goh_flush_archived_values(nHanle, point_id, &flush_count);
		g_logger->info("Flush datas, id={}, count={}", point_id, flush_count);
		CHECK_ECODE(ecode, fmt::format("Flush datas, id={}", point_id).c_str(), g_logger);
	}

	return GoE_OK;
}

golden_error put_same_time_thread(int pos, int count, bool show_progress)
{
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

	auto& start_t_s = golden_config::start_time_int_;
	start_t_s = (start_t_s == -1) ? (g_snapshot_time[pos] + 1) : start_t_s;
	auto& end_t_s = golden_config::end_time_int_;
	auto& increment_t_ms = golden_config::increment_time_;
	//auto increment_t_s = increment_t_ms / 1000;
	bool timestamp_is_second = (increment_t_ms % 1000 == 0) ? true : false;

	//填充数据
	int batch_count = count;
	vector<golden_int32> datetimes(batch_count);
	vector<golden_int16> ms(batch_count, 0);
	vector<golden_int16> quality(batch_count, GOLDEN_Q_GOOD);
	vector<golden_error> errors(batch_count, 0);
	//值，换别名，针对不同类型选择不同的基础数据
	auto&& fvalues = g_vvfvalues;
	auto&& ivalues = g_vvxvalues;
	//函数封装，决定写快照还是写历史
	std::function<golden_error(golden_int32, golden_int32*, const golden_int32 *, const golden_int32 *, const golden_int16 *, const golden_float64 *, const golden_int64 *, const golden_int16 *, golden_error *)> put_datas = golden_config::put_history_data_ ? goh_put_archived_values : gos_put_snapshots;

	golden_int64 lasttime_ms = (golden_int64)start_t_s * 1000;
	golden_int64 endtime_ms = (golden_int64)end_t_s * 1000 + 999;
	stop_watch api_watch;
	auto sleep_until_time = std::chrono::milliseconds(golden_config::elapse_time_);  // 控制等待间隔
	auto sleep_time_if_not_ready = std::chrono::seconds(5);
	double elapsed_time = 0.0;              // 总耗时
	unsigned long long total_count = 0;     // 总数据量
	int source_data_position = 0;           // 从数据源获取数据的位置，在一个周期内循环
	golden_int32 put_count = (golden_int32)((endtime_ms - lasttime_ms + 1) / increment_t_ms);     // 写入次数
	golden_int32 completed_count = 1;

	while (lasttime_ms < endtime_ms)
	{
		auto&& abs_time = std::chrono::system_clock::now();
		std::fill(datetimes.begin(), datetimes.end(), (golden_int32)(lasttime_ms / 1000));
		if (!timestamp_is_second)
			std::fill(ms.begin(), ms.end(), (golden_int16)(lasttime_ms % 1000));
		std::fill(errors.begin(), errors.end(), (golden_error)0);

		golden_int32 put_value_count = batch_count;
	PUT_DATAS:
		auto put_value_count_successful = put_value_count;
		api_watch.restart();
		ecode = put_datas(nHanle, &put_value_count_successful, g_ids.data() + pos, datetimes.data(), ms.data(),
			fvalues.at(source_data_position).data() + pos, ivalues.at(source_data_position).data() + pos, quality.data(), errors.data());
		if (ecode == GoE_ARV_PAGE_NOT_READY || ecode == GoE_ARVEX_PAGE_NOT_READY) {
			g_logger->warn("The database cache is full, wait 5 seconds to try again.");
			this_thread::sleep_for(sleep_time_if_not_ready);
			goto PUT_DATAS;
		}
		api_watch.stop();
		g_logger->info("Put datas batch: start_id={}, point_count={}, successful={}, timestamp={}ms, ret=0x{:x}, elapsed={}ms",
			g_ids.at(pos), put_value_count, put_value_count_successful, lasttime_ms, ecode, api_watch.elapsed_ms());
		CHECK_ECODE(ecode, fmt::format("Put datas, start_id={}, point_count={}, timestamp={}ms", g_ids.at(pos), put_value_count, lasttime_ms).c_str(), g_logger);
		lasttime_ms += increment_t_ms;
		source_data_position = (source_data_position + 1) % golden_config::func_period_;
		elapsed_time += api_watch.elapsed_ms();
		total_count += put_value_count;
		if (show_progress) {
			g_progress->critical("progress : {:.2f}%", (double)completed_count * 100 / put_count);
			++completed_count;
		}
		abs_time += sleep_until_time;
		if (abs_time > std::chrono::system_clock::now()) {
			this_thread::sleep_until(abs_time);
		}
		if (ecode != GoE_OK) {
			return ecode;
		}
	}
	g_logger->warn("Put datas summary : start_id={}, point_count={}, put_count={}, total={}, elapsed={}ms, avg_elapsed={}ms, count_per_sec={}",
		g_ids.at(pos), count, put_count, total_count, elapsed_time, elapsed_time / put_count, (double)(total_count * 1000) / elapsed_time);
	all_thread_single_call_avg_elapsed += (elapsed_time / put_count);
	g_make_total_count += total_count;

	//将所有标签点未写满的补历史缓存页写入存档文件中
	//如果需要立即查看数据，可以在程序退出时调用该函数，
	//运行中频繁调用会降低数据库性能
	int flush_count = 0;
	if (golden_config::put_history_data_) {
		for (int i = pos; i < pos + count; ++i)
		{
			ecode = goh_flush_archived_values(nHanle, g_ids.at(i), &flush_count);
			g_logger->info("Flush datas, id={}, count={}", g_ids.at(i), flush_count);
		}
	}

	return GoE_OK;
}

// 生成数据
bool generate_data()
{
	if (0 == golden_config::write_mode_.compare("point")) {
		// 查询线程池
		g_thread_pool.init(golden_config::threadcount_);
		vector<std::future<golden_error> >results;
		int completed_count = 1;
		for (int i = 0; i < golden_config::point_count_; ++i) {
			results.emplace_back(g_thread_pool.enqueue(put_same_point_thread, i));
		}
		for (auto&& result : results) {
			result.get();
			g_progress->critical("progress : {:.2f}%", (double)completed_count * 100 / golden_config::point_count_);
			++completed_count;
			if (!g_running) break;
		}
		g_thread_pool.close();
		g_logger->critical("The average time to write single point all time data is {:.2f}ms, and total value count is {}.", all_thread_all_call_total_elapsed / golden_config::point_count_, g_make_total_count);
	}
	else {  // write_mode_ == time
		golden_config::threadcount_ = min(golden_config::point_count_, golden_config::threadcount_);     // 线程数不能大于标签点数
		int remaining_count = golden_config::point_count_ % golden_config::threadcount_;          // 按线程数分组之后剩余的个数
		int point_count_per_thread = golden_config::point_count_ / golden_config::threadcount_;   // 每个线程负责的标签点数
		g_thread_pool.init(golden_config::threadcount_);
		vector<std::future<golden_error> >results;
		bool show_progress = true;
		for (int i = 0; i < golden_config::threadcount_; ++i) {
			results.emplace_back(g_thread_pool.enqueue(put_same_time_thread, i * point_count_per_thread, ((remaining_count && (golden_config::threadcount_ - 1 == i)) ? remaining_count : point_count_per_thread), show_progress));
			show_progress = false;
			this_thread::sleep_for(std::chrono::seconds(1));  // 现场测试，错开启动时间，避免出现所有数据页同时写满的情况
		}
		for (auto&& result : results) {
			result.get();
			if (!g_running) break;
		}
		g_thread_pool.close();
		g_logger->critical("The average time to write all point single time data is {:.2f}ms, and total value count is {}.", all_thread_single_call_avg_elapsed / golden_config::threadcount_, g_make_total_count);
	}
	if (!golden_config::result_file_.empty()) {
		try {
			spdlog::details::file_helper file_helper;
			file_helper.open(golden_config::result_file_);
			fmt::memory_buffer buffer;
			fmt::writer writer(buffer);
			auto fmt_specs = fmt::basic_format_specs<char>();
			fmt_specs.precision_ = -3;  // 浮点数保留3位小数
			auto *name_ptr = golden_config::task_name_.data();
			writer.write(std::string("task_name,put_one_point_full_data_elapsed,put_one_time_full_data_elapsed,make_total_count\n"));
			buffer.append(name_ptr, name_ptr + golden_config::task_name_.size());
			buffer.push_back(',');
			writer.write(all_thread_all_call_total_elapsed / golden_config::point_count_, fmt_specs);
			buffer.push_back(',');
			writer.write(all_thread_single_call_avg_elapsed / golden_config::threadcount_, fmt_specs);
			buffer.push_back(',');
			writer.write(g_make_total_count);
			buffer.push_back('\n');
			file_helper.write(buffer);
			file_helper.flush();
		}
		catch (const spd::spdlog_ex& e) {
			g_logger->critical("{}", e.what());
			system("pause");
			return false;
		}
	}
	return true;
}

int main(int argc, char *argv[])
{
	std::cout << ",-,-,-.       .         .-,--.      .      " << endl;
	std::cout << "`,| | |   ,-. | , ,-.   ' |   \\ ,-. |- ,-. " << endl;
	std::cout << "  | ; | . ,-| |<  |-'   , |   / ,-| |  ,-| " << endl;
	std::cout << "  '   `-' `-^ ' ` `-'   `-^--'  `-^ `' `-^ " << endl;
	std::cout << "                                           " << endl;
	std::cout << "                                           " << endl << endl;
	// http://patorjk.com/software/taag/#p=display&f=Stampatello&t=Golden%20Make%20Data

	CLI::App app{ "App description" };
	{
		golden_config::task_name_ = "main";
		app.add_option("-n,--taskname", golden_config::task_name_, "task name (=main)")->required();
		golden_config::host_name_;
		app.add_option("-a,--address", golden_config::host_name_, "host name (=127.0.0.1)\n You can set up multi-IP addresses separated by spaces to take advantage of the server's multi-network card.\n e.g. -a 192.168.0.2 192.168.0.3 192.168.0.4 192.168.0.5");
		golden_config::port_ = 6327;
		app.add_option("-p,--port", golden_config::port_, "port number (=6327)");
		golden_config::user_ = "sa";
		app.add_option("-u,--user", golden_config::user_, "user name (=sa)");
		golden_config::password_ = "admin";
		app.add_option("-w,--password", golden_config::password_, "pass word (=admin)");
		golden_config::start_time_ = "now";
		app.add_option("-s,--starttime", golden_config::start_time_, "start time (=now)\nformat:\n \"YYYY-MM-DD hh:mm:ss\"\n \"maxtime\" start time is read snapshot max datetime\n \"now\" start time is local real time\n Enclosed in single or double quotes.");
		golden_config::end_time_ = "forever";
		app.add_option("-e,--endtime", golden_config::end_time_, "end time (=forever)\nformat:\n \"YYYY-MM-DD hh:mm:ss\"\n \"forever\" end time is max UTC time");
		golden_config::elapse_time_ = 1000;
		app.add_option("-E,--elapsetime", golden_config::elapse_time_, "elpase time (=1000) ms");
		golden_config::increment_time_ = 1000;
		app.add_option("-i,--increment", golden_config::increment_time_, "increment time (=1000) ms");
		golden_config::put_history_data_ = false;
		app.add_flag("-H,--history", golden_config::put_history_data_, "put history data");
		golden_config::min_value_ = -100;
		app.add_option("--low", golden_config::min_value_, "min value (=-100)");
		golden_config::max_value_ = 100;
		app.add_option("--high", golden_config::max_value_, "max value (=100)");
		golden_config::generator_ = "sin";
		app.add_option("-g,--generator", golden_config::generator_, "data generator (=sin), sin, line, rand, file");
#ifdef _WIN32
		golden_config::search_condition_ = "";
		app.add_option("--search", golden_config::search_condition_, "search condition");
#endif //_WIN32
		golden_config::first_point_ = 1;
		app.add_option("--first_point", golden_config::first_point_, "first point's id (=1)");
		golden_config::point_count_ = 1;
		app.add_option("--point_count", golden_config::point_count_, "point count (=1)");
		golden_config::point_interval_ = 1;
		app.add_option("--point_interval", golden_config::point_interval_, "point interval (=1)");
		golden_config::write_mode_ = "time";
		app.add_option("--write_mode", golden_config::write_mode_, "write mode (=time)\n time : same time once\n point : same point once");
		golden_config::threadcount_ = 1;
		app.add_option("--thread_count", golden_config::threadcount_, "thread count (=1)");
		golden_config::print_log_ = true;
		app.add_flag("--print_log", golden_config::print_log_, "print log to console");
		golden_config::log_level_ = spdlog::level::level_enum::info;
		app.add_option("--log_level", golden_config::log_level_, "log level (=2) as info\n 0.trace\n 1.debug\n 2.info\n 3.warn\n 4.err\n 5.critical\n 6.off");
		golden_config::func_period_ = 36;
		app.add_option("--func_period", golden_config::func_period_, "generator function period (=36)\n  when write_mode=point, write func_period*100 values count per batch, so set 3600 or more is better.");
		app.add_option("--result_file", golden_config::result_file_, "result file path, default is empty.");
#ifdef _WIN32
		app.add_flag("--Attention", "# 注意：\n#   1.当write_mode = point时，开始时间到结束时间的跨度不宜太长，取决于时间跨度内的文件数量，建议不要超过100个，可以多写几个命令，顺序执行\n#   2.当write_mode = point时，增加参数--func_period 3600，每个点每批写入func_period * 100个，也就是同一个波形重复100次，调整这个参数可以控制每批写入的数据量，这里占用内存很少\n#   3.当write_mode = time时，参数--func_period默认为36，会申请 2 * 8 * point_count*func_period 的内存，如果点数过多的话，会占用大量内存，故不宜设置太大，具体占多少内存合适，可以用top命令查看");
#elif _LINUX
		app.add_flag("--Attention", "Attention:\n1.When write_mode = point, The start-to-end time span should not be too long,\n depending on the number of archive files in the time span.\n It is not recommended to exceed 100.\n You can write a few more commands and execute them sequentially.\n2.When write_mode = point, Add the parameter --func_period 3600.\n Write func_period * 100 values per batch per point,\n i.e. 100 repetitions of the same waveform.\n Adjusting this parameter controls the amount of data written per batch,\n and it uses very little memory.\n3.When write_mode = time, Add the parameter --func_period 36.\n 2 * 8 * point_count * func_period Byte memory will be requested.\n If you have too many points, it will take up a lot of memory,\n so it is not appropriate to set too much.\n You can view it using the top command.");
#endif

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

	g_logger->trace("Generating data.");
	if (!generate_data())
		return -1;

	g_logger->info("Exit the app.");
    return 0;
}