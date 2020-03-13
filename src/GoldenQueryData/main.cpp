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
	std::string task_name_;
	std::vector<std::string> host_name_;
	int port_;
	std::string user_;
	std::string password_;
	std::string start_time_;
	std::string end_time_;
	std::string search_condition_;
	int first_point_;
	int point_count_;
	int point_interval_;
	std::string query_mode_;
	int query_batch_count_;
	int interval_;
	int thread_count_;
	int thread1_count_;
	bool print_log_;
	int log_level_;
	std::string result_file_;

	int start_time_int_;
	short start_time_ms_;
	int end_time_int_;
	short end_time_ms_;
};

enum data_type_e
{
	no_support = 0,
	analog = 1,
	digital = 2,
	boolean = 3
};

// ���ӳ�
queue<golden_int32> connect_queue;
std::mutex cq_mutex;

// ��־
std::shared_ptr<spdlog::logger> g_logger = nullptr;
std::shared_ptr<spdlog::logger> g_progress = nullptr;

std::string g_app_root_path;
std::string app_name = "query_data";
bool g_running = false;
static thread_pool g_thread_pool;
static thread_pool g_thread_pool1;
double all_thread_all_call_total_elapsed = 0.0;    // �����߳����е��õ��ܺ�ʱ
double all_thread_single_call_avg_elapsed = 0.0;   // �����̵߳��ε���ƽ����ʱ
unsigned long long g_query_total_count = 0;        // ��ѯ����������

// ���ݿ�
std::vector<int> g_ids;
std::vector<int> g_types;
std::vector<golden_byte> g_usems;
std::vector<data_type_e> g_types_simple;

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

// �ж���������
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
			//stdout_logger_ = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
			//assert(stdout_logger_);
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
				if (pot_pos != string::npos) {
					char *p_s= (char*)golden_config::start_time_.data();
					*(p_s + pot_pos) = '\0';
					go_parse_time(p_s, &_start_time, &_start_time_ms);
					*(p_s + pot_pos) = '.';
					char* p_ms = p_s + pot_pos + 1;
					_start_time_ms = (short)atoi(p_ms);
				}
				else {
					go_parse_time(golden_config::start_time_.c_str(), &_start_time, &_start_time_ms);
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
				if (pot_pos != string::npos) {
					char *p_s = (char*)golden_config::end_time_.data();
					*(p_s + pot_pos) = '\0';
					go_parse_time(p_s, &_end_time, &_end_time_ms);
					*(p_s + pot_pos) = '.';
					char* p_ms = p_s + pot_pos + 1;
					_end_time_ms = (short)atoi(p_ms);
				}
				else {
					go_parse_time(golden_config::end_time_.c_str(), &_end_time, &_end_time_ms);
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
	// ����������ʼ��ID���飬��first_point_������point_interval_�ȼ������
	g_logger->trace("{}:begin.", __FUNCTION__);

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


	vector<golden_error> errors(golden_config::point_count_);
	int point_count_tmp = golden_config::point_count_;
	g_logger->trace("{}:alloc snapshot mem.", __FUNCTION__);

	// ��ȡ��ǩ����������
	ecode = gob_get_types_prpperty(nHanle, &point_count_tmp, g_ids.data(), g_types.data(), errors.data());
	if (ecode != GoE_OK) {
		check_ecode(ecode, "Get type property", g_logger);
		return false;
	}
	// ��ǩ������ת��
	std::generate(g_types_simple.begin(), g_types_simple.end(), [n = 0]() mutable { return judge_data_type(g_types[n++]); });

	g_usems.resize(golden_config::point_count_);
	// ��ȡ��ǩ���Ƿ����
	ecode = gob_get_ms_prpperty(nHanle, &point_count_tmp, g_ids.data(), g_usems.data(), errors.data());
	if (ecode != GoE_OK) {
		check_ecode(ecode, "Get ms property", g_logger);
		return false;
	}
	
	// �������ݿ������ѯ����
	ecode = go_set_db_info2(nHanle, GOLDEN_PARAM_ARCHIVE_BATCH_SIZE, golden_config::query_batch_count_);
	if (ecode != GoE_OK) {
		CHECK_ECODE(ecode, "Set Param: Query archived values batch size.", g_logger);
		return false;
	}

	return true;
}

golden_error get_history_archived_thread(int pos, int count)
{
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

	auto& start_t_s = golden_config::start_time_int_;
	auto& start_t_ms = golden_config::start_time_ms_;
	auto& end_t_s = golden_config::end_time_int_;
	auto& end_t_ms = golden_config::end_time_ms_;

	vector<golden_int32> datetimes(golden_config::query_batch_count_);
	vector<golden_int16> ms(golden_config::query_batch_count_);
	vector<golden_float64> fvalues(golden_config::query_batch_count_);
	vector<golden_int64> ivalues(golden_config::query_batch_count_);
	vector<golden_int16> quality(golden_config::query_batch_count_);

	stop_watch total_point_watch, one_point_watch, single_call_watch;
	double elapsed_time = 0.0;
	unsigned long long all_point_total_count = 0;

	total_point_watch.restart();
	std::for_each(g_ids.begin() + pos, g_ids.begin() + pos + count, [&](int id) {
		int value_count = 0, one_point_total_count = 0, batch_count = golden_config::query_batch_count_, batch_num = 1;
		ecode = goh_get_archived_values_in_batches(nHanle, id, start_t_s, start_t_ms, end_t_s, end_t_ms, &value_count, &batch_count);
		one_point_watch.restart();
		CHECK_ECODE(ecode, fmt::format("Get datas, id={}", id).c_str(), g_logger);
		do
		{
			batch_count = golden_config::query_batch_count_;
			single_call_watch.restart();
			ecode = goh_get_next_archived_values(nHanle, id, &batch_count, datetimes.data(), ms.data(), fvalues.data(), ivalues.data(), quality.data());
			single_call_watch.stop();
			if (ecode == GoE_BATCH_END) //����API��BUG�����·��ص�batch_count����ȷ����ʱ����
				batch_count = value_count - one_point_total_count;
			one_point_total_count += batch_count;
			g_logger->trace("Get batch[{}] datas : id={}, batch_count={}, start_time={}, end_time={}, ret=0x{:x}, elapsed={}ms", batch_num++, id, batch_count, cast_time_ms_str(datetimes[0], ms[0]), cast_time_ms_str(datetimes[batch_count-1], ms[batch_count-1]), ecode, single_call_watch.elapsed_ms());
			//if (ecode == GoE_BATCH_END) {
			//	for (int i = 0; i < batch_count; ++i) {
			//		fmt::print("{}\tid={}, datetime={}, ms={}, fvalue={}, ivalue={}, quality={}\n", i + 1, id, datetimes[i], ms[i], fvalues[i], ivalues[i], quality[i]);
			//	}
			//	fmt::print("start_t_s = {}, end_t_s = {}\n", start_t_s, end_t_s);
			//}
		} while ((ecode != GoE_BATCH_END) && (one_point_total_count < value_count));
		one_point_watch.stop();
		g_logger->warn("Get datas : id={}, query_total_count={}, real_total_count={}, elapsed={}ms",
			id, value_count, one_point_total_count, one_point_watch.elapsed_ms());
		all_point_total_count += (unsigned long long)one_point_total_count;
	});
	total_point_watch.stop();
	elapsed_time = total_point_watch.elapsed_ms();
	g_logger->warn("Get datas summary : first_id={}, id_count={}, total={}, elapsed={}ms, count_per_sec={}",
		g_ids.at(pos), count, all_point_total_count, elapsed_time, (double)(all_point_total_count * 1000) / elapsed_time);
	all_thread_all_call_total_elapsed += elapsed_time;
	g_query_total_count += all_point_total_count;

	return GoE_OK;
}

golden_error get_history_archived_ex_thread(int pos, int count)
{
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

	vector<golden_int32> datetimes(golden_config::query_batch_count_);
	vector<golden_int16> ms(golden_config::query_batch_count_);
	vector<golden_float64> fvalues(golden_config::query_batch_count_);
	vector<golden_int64> ivalues(golden_config::query_batch_count_);
	vector<golden_int16> quality(golden_config::query_batch_count_);

	stop_watch total_point_watch, one_point_watch, single_call_watch;
	double elapsed_time = 0.0;
	unsigned long long all_point_total_count = 0;

	total_point_watch.restart();
	for (int index = pos; index < pos + count; ++index) {
		int id = g_ids.at(index);
		golden_byte usems = g_usems.at(index);
		auto start_t_s = golden_config::start_time_int_;
		auto start_t_ms = golden_config::start_time_ms_;
		auto end_t_s = golden_config::end_time_int_;
		auto end_t_ms = golden_config::end_time_ms_;
		int one_point_total_count = 0, batch_count = golden_config::query_batch_count_, batch_num = 1;
		one_point_watch.restart();
		while (batch_count == golden_config::query_batch_count_ && !GOLDEN_TIME_GREATER_THAN(start_t_s, start_t_ms, end_t_s, end_t_ms))
		{
			// ���±�����ѯ��ʼ����ʱ��
			datetimes[0] = start_t_s;
			datetimes[batch_count - 1] = end_t_s;
			ms[0] = start_t_ms;
			ms[batch_count - 1] = end_t_ms;
			g_logger->trace("Begin batch[{}] datas : id={}, batch_count={}, start_time={}, end_time={}", batch_num, id, batch_count, cast_time_ms_str(datetimes[0], ms[0]), cast_time_ms_str(datetimes[batch_count - 1], ms[batch_count - 1]));
			// ��ʼ��ѯ
			single_call_watch.restart();
			ecode = goh_get_archived_values(nHanle, id, &batch_count, datetimes.data(), ms.data(), fvalues.data(), ivalues.data(), quality.data());
			single_call_watch.stop();
			if (batch_count > 0) {
				one_point_total_count += batch_count;
				g_logger->trace("Get batch[{}] datas : id={}, batch_count={}, start_time={}, end_time={}, ret=0x{:x}, elapsed={}ms", batch_num++, id, batch_count, cast_time_ms_str(datetimes[0], ms[0]), cast_time_ms_str(datetimes[batch_count-1], ms[batch_count-1]), ecode, single_call_watch.elapsed_ms()); 
				// ����ʱ�䷶Χ
				if (datetimes[batch_count - 1] >= start_t_s) {
					if (usems) {
						start_t_s = datetimes[batch_count - 1];
						start_t_ms = ms[batch_count - 1] + short(1);
						if (start_t_ms > 999) {
							start_t_s++;
							start_t_ms = 0;
						}
					}
					else {
						start_t_s = datetimes[batch_count - 1] + 1;
						start_t_ms = 0;
					}
				}
				else
					break;
			}
		}
		one_point_watch.stop();
		g_logger->warn("Get datas : id={}, query_total_count={}, elapsed={}ms",
			id, one_point_total_count, one_point_watch.elapsed_ms());
		all_point_total_count += (unsigned long long)one_point_total_count;
	}
	total_point_watch.stop();
	elapsed_time = total_point_watch.elapsed_ms();
	g_logger->warn("Get datas summary : first_id={}, id_count={}, total={}, elapsed={}ms, count_per_sec={}",
		g_ids.at(pos), count, all_point_total_count, elapsed_time, (double)(all_point_total_count * 1000) / elapsed_time);
	all_thread_all_call_total_elapsed += elapsed_time;
	g_query_total_count += all_point_total_count;

	return GoE_OK;
}

golden_error get_snapshot_value_thread(int pos, int count)
{
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

	vector<golden_int32> ids(count);
	vector<golden_int32> datetimes(count);
	vector<golden_int16> ms(count);
	vector<golden_float64> fvalues(count);
	vector<golden_int64> ivalues(count);
	vector<golden_int16> quality(count);
	vector<golden_error> errors(count);

	stop_watch total_point_watch;
	double elapsed_time = 0.0;
	for (int index = pos; index < pos + count; ++index) {
		int id = g_ids.at(index);
	}
	total_point_watch.restart();
	ecode = gos_get_snapshots(nHanle, &count, ids.data(), datetimes.data(), ms.data(), fvalues.data(), ivalues.data(), quality.data(), errors.data());
	total_point_watch.stop();
	elapsed_time = total_point_watch.elapsed_ms();
	g_logger->warn("Get datas : first_id={}, id_count={}, elapsed={}ms",
		g_ids.at(pos), count, elapsed_time);
	all_thread_all_call_total_elapsed += elapsed_time;
	g_query_total_count += count;

	return GoE_OK;
}

golden_error get_plot_value_thread(int pos, int count)
{
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

	auto& start_t_s = golden_config::start_time_int_;
	auto& start_t_ms = golden_config::start_time_ms_;
	auto& end_t_s = golden_config::end_time_int_;
	auto& end_t_ms = golden_config::end_time_ms_;
	auto& interval = golden_config::interval_;
	int query_count = interval * 5;

	vector<golden_int32> datetimes(query_count);
	vector<golden_int16> ms(query_count);
	vector<golden_float64> fvalues(query_count);
	vector<golden_int64> ivalues(query_count);
	vector<golden_int16> quality(query_count);

	stop_watch total_point_watch, one_point_watch, single_call_watch;
	double elapsed_time = 0.0;
	unsigned long long all_point_total_count = 0;

	total_point_watch.restart();
	std::for_each(g_ids.begin() + pos, g_ids.begin() + pos + count, [&](int id) {
		int value_count = query_count;
		datetimes[0] = start_t_s;    ms[0] = start_t_ms;
		datetimes[value_count - 1] = end_t_s;   ms[value_count - 1] = end_t_ms;
		one_point_watch.restart();
		ecode = goh_get_plot_values(nHanle, id, interval, &value_count, datetimes.data(), ms.data(), fvalues.data(), ivalues.data(), quality.data());
		one_point_watch.stop();
		CHECK_ECODE(ecode, fmt::format("Get data, id={}", id).c_str(), g_logger);
		//for (int i = 0; i < value_count; ++i) {
		//	fmt::print("{}\tid={}, datetime={}, ms={}, fvalue={}, ivalue={}, quality={}", i + 1, id, datetimes[i], ms[i], fvalues[i], ivalues[i], quality[i]);
		//}
		g_logger->warn("Get data : id={}, interval={}, value_count={}, elapsed={:.2f}ms",
			id, interval, value_count, one_point_watch.elapsed_ms());
		all_point_total_count += (unsigned long long)value_count;
	});
	total_point_watch.stop();
	elapsed_time = total_point_watch.elapsed_ms();
	g_logger->warn("Get data summary : first_id={}, id_count={}, total={}, elapsed={:.2f}ms",
		g_ids.at(pos), count, all_point_total_count, elapsed_time);
	all_thread_all_call_total_elapsed += elapsed_time;
	g_query_total_count += all_point_total_count;

	return GoE_OK;
}

golden_error get_interval_value_thread(int pos, int count)
{
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

	auto& start_t_s = golden_config::start_time_int_;
	auto& start_t_ms = golden_config::start_time_ms_;
	unsigned long long start_t = (unsigned long long)start_t_s * 1000 + start_t_ms;
	auto& end_t_s = golden_config::end_time_int_;
	auto& end_t_ms = golden_config::end_time_ms_;
	unsigned long long end_t = (unsigned long long)end_t_s * 1000 + end_t_ms;
	auto& interval_t_ms = golden_config::interval_;
	auto interval_t_s = interval_t_ms / 1000;
	bool timestamp_is_second = (interval_t_ms % 1000 == 0) ? true : false;

	int query_count = 1 + timestamp_is_second ? (int)floor(double(start_t_s - end_t_s)/ interval_t_s) : (int)floor((double)(end_t - start_t) / interval_t_ms);

	vector<golden_int32> datetimes(query_count);
	vector<golden_int16> ms(query_count);
	vector<golden_float64> fvalues(query_count);
	vector<golden_int64> ivalues(query_count);
	vector<golden_int16> quality(query_count);

	if (timestamp_is_second) {
		std::generate(datetimes.begin(), datetimes.end(), [n = 0, &start_t_s, &interval_t_s]() mutable { 
			return start_t_s + (interval_t_s * n++); });
	}
	else {
		datetimes.at(0) = start_t_s;
		ms.at(0) = start_t_ms;
		for (int i = 0; i < query_count - 1; ++i) {
			int&& forward_step_ms = (int)ms.at(i) + interval_t_ms;
			datetimes[i + 1] = datetimes[i] + forward_step_ms / 1000;
			ms[i + 1] = ms[i] + (short)(forward_step_ms % 1000);
		}
	}

	stop_watch total_point_watch, one_point_watch, single_call_watch;
	double elapsed_time = 0.0;
	unsigned long long all_point_total_count = 0;

	total_point_watch.restart();
	std::for_each(g_ids.begin() + pos, g_ids.begin() + pos + count, [&](int id) {
		one_point_watch.restart();
		ecode = goh_get_timed_values(nHanle, id, query_count, datetimes.data(), ms.data(), fvalues.data(), ivalues.data(), quality.data());
		one_point_watch.stop();
		CHECK_ECODE(ecode, fmt::format("Get data, id={}", id).c_str(), g_logger);
		//for (int i = 0; i < value_count; ++i) {
		//	fmt::print("{}\tid={}, datetime={}, ms={}, fvalue={}, ivalue={}, quality={}", i + 1, id, datetimes[i], ms[i], fvalues[i], ivalues[i], quality[i]);
		//}
		g_logger->warn("Get data : id={}, interval={}, value_count={}, elapsed={:.2f}ms",
			id, interval_t_ms, query_count, one_point_watch.elapsed_ms());
		all_point_total_count += (unsigned long long)query_count;
	});
	total_point_watch.stop();
	elapsed_time = total_point_watch.elapsed_ms();
	g_logger->warn("Get data summary : first_id={}, id_count={}, total={}, elapsed={:.2f}ms",
		g_ids.at(pos), count, all_point_total_count, elapsed_time);
	all_thread_all_call_total_elapsed += elapsed_time;
	g_query_total_count += all_point_total_count;

	return GoE_OK;
}

golden_error get_summary_value_thread(int pos, int count)
{
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

	auto& start_t_s = golden_config::start_time_int_;
	auto& start_t_ms = golden_config::start_time_ms_;
	auto& end_t_s = golden_config::end_time_int_;
	auto& end_t_ms = golden_config::end_time_ms_;

	golden_int32 _start_t_s = start_t_s;
	golden_int16 _start_t_ms = start_t_ms;
	golden_int32 _end_t_s = end_t_s;
	golden_int16 _end_t_ms = end_t_ms;

	golden_float64 max_value;
	golden_float64 min_value;
	golden_float64 total_value;
	golden_float64 calc_avg;
	golden_float64 power_avg;

	stop_watch total_point_watch, one_point_watch, single_call_watch;
	double elapsed_time = 0.0;
	unsigned long long all_point_total_count = 0;

	total_point_watch.restart();
	std::for_each(g_ids.begin() + pos, g_ids.begin() + pos + count, [&](int id) {
		_start_t_s = start_t_s;
		_start_t_ms = start_t_ms;
		_end_t_s = end_t_s;
		_end_t_ms = end_t_ms;
		one_point_watch.restart();
		ecode = goh_summary(nHanle, id, &_start_t_s, &_start_t_ms, &_end_t_s, &_end_t_ms, &max_value, &min_value, &total_value, &calc_avg, &power_avg);
		one_point_watch.stop();
		CHECK_ECODE(ecode, fmt::format("Get data, id={}", id).c_str(), g_logger);
		g_logger->warn("Get data : id={}, max_value={:.2f}, min_value={:.2f}, total_value={:.2f}, calc_avg={:.2f}, power_avg={:.2f}, elapsed={:.2f}ms",
			id, max_value, min_value, total_value, calc_avg, power_avg, one_point_watch.elapsed_ms());
		all_point_total_count += 5ULL;
	});
	total_point_watch.stop();
	elapsed_time = total_point_watch.elapsed_ms();
	g_logger->warn("Get data summary : first_id={}, id_count={}, total={}, elapsed={:.2f}ms",
		g_ids.at(pos), count, all_point_total_count, elapsed_time);
	all_thread_all_call_total_elapsed += elapsed_time;
	g_query_total_count += all_point_total_count;

	return GoE_OK;
}

golden_error get_section_summary_value_thread(int pos, int count)
{
	auto& start_t_s = golden_config::start_time_int_;
	auto& start_t_ms = golden_config::start_time_ms_;
	auto& end_t_s = golden_config::end_time_int_;
	auto& end_t_ms = golden_config::end_time_ms_;

	golden_int32 _start_t_s = start_t_s;
	golden_int16 _start_t_ms = start_t_ms;
	golden_int32 _end_t_s = end_t_s;
	golden_int16 _end_t_ms = end_t_ms;

	stop_watch total_point_watch, one_point_watch, single_call_watch;
	double elapsed_time = 0.0;
	unsigned long long all_point_total_count = 0;

	auto get_summary_value = [&](int id, golden_int32 start_t_s, golden_int16 start_t_ms, golden_int32 end_t_s, golden_int16 end_t_ms)->golden_error
	{
		stop_watch one_point_watch;
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
		golden_float64 max_value;
		golden_float64 min_value;
		golden_float64 total_value;
		golden_float64 calc_avg;
		golden_float64 power_avg;
		golden_int32 _start_t_s = start_t_s;
		golden_int16 _start_t_ms = start_t_ms;
		golden_int32 _end_t_s = end_t_s;
		golden_int16 _end_t_ms = end_t_ms;
		golden_error ecode = GoE_OK;
		int remaining_time = (_end_t_s - _start_t_s + 1) % golden_config::interval_;
		int time_count_per_thread = (_end_t_s - _start_t_s + 1) / golden_config::interval_ + (0 != remaining_time ? 1 : 0);
		for (int i = 0; i < time_count_per_thread; ++i) {
			start_t_s = start_t_s + i * golden_config::interval_;
			if (remaining_time && (time_count_per_thread - 1 == i))
			{
				_end_t_s = end_t_s;
			}
			else
			{
				_end_t_s = start_t_s + (i + 1) * golden_config::interval_ - 1;
			}
			one_point_watch.restart();
			ecode = goh_summary(nHanle, id, &_start_t_s, &_start_t_ms, &_end_t_s, &_end_t_ms, &max_value, &min_value, &total_value, &calc_avg, &power_avg);
			one_point_watch.stop();
			CHECK_ECODE(ecode, fmt::format("Get data, id={}", id).c_str(), g_logger);
			g_logger->warn("Get data : id={}, max_value={:.2f}, min_value={:.2f}, total_value={:.2f}, calc_avg={:.2f}, power_avg={:.2f}, elapsed={:.2f}ms",
				id, max_value, min_value, total_value, calc_avg, power_avg, one_point_watch.elapsed_ms());
		}
		return GoE_OK;
	};
	g_thread_pool1.init(golden_config::thread1_count_);
	total_point_watch.restart();
	std::for_each(g_ids.begin() + pos, g_ids.begin() + pos + count, [&](int id) {
		one_point_watch.restart();
		vector<std::future<golden_error> >results;
		int remaining_time = (_end_t_s - _start_t_s + 1) % golden_config::thread1_count_;
		int time_count_per_thread = (_end_t_s - _start_t_s + 1) / golden_config::thread1_count_;
		for (int i = 0; i < golden_config::thread1_count_; ++i) {
			_start_t_s = start_t_s + i * time_count_per_thread;
			_start_t_ms = 0;
			_end_t_ms = 0;
			if (remaining_time && (golden_config::thread1_count_ - 1 == i))
			{
				_end_t_s = end_t_s;
			}
			else
			{
				_end_t_s = start_t_s + (i + 1) * time_count_per_thread - 1;
			}
			results.emplace_back(g_thread_pool1.enqueue(get_summary_value, id, _start_t_s, _start_t_ms, _end_t_s, _end_t_ms));	
		}
		for (auto&& result : results) {
			result.get();
			if (!g_running) break;
		}
		
		all_point_total_count += 5ULL;
	});
	total_point_watch.stop();
	elapsed_time = total_point_watch.elapsed_ms();
	g_logger->warn("Get data summary : first_id={}, id_count={}, total={}, elapsed={:.2f}ms",
		g_ids.at(pos), count, all_point_total_count, elapsed_time);
	all_thread_all_call_total_elapsed += elapsed_time;
	g_query_total_count += all_point_total_count;
	g_thread_pool1.close();

	return GoE_OK;
}

//golden_error get_corss_section_value_thread(int pos, int count, bool show_progress)
//{
//	golden_error ecode = GoE_OK;
//	int nHanle = 0;
//	{
//		// �����ӳػ�ȡ���Ӿ��
//		std::unique_lock<std::mutex> lock(cq_mutex);
//		g_logger->trace("Get connection handle, current connection pool size : {}", connect_queue.size());
//		nHanle = connect_queue.front();
//		connect_queue.pop();
//	}
//	ON_SCOPE_EXIT([&] {
//		if (nHanle) {
//			std::unique_lock<std::mutex> lock(cq_mutex);
//			connect_queue.push(nHanle);
//			g_logger->trace("Return connection handle : {}, current connection pool size : {}", nHanle, connect_queue.size());
//		}
//	});
//
//	auto& start_t_s = golden_config::start_time_int_;
//	auto& start_t_ms = golden_config::start_time_ms_;
//	unsigned long long start_t = (unsigned long long)start_t_s * 1000 + start_t_ms;
//	auto& end_t_s = golden_config::end_time_int_;
//	auto& end_t_ms = golden_config::end_time_ms_;
//	unsigned long long end_t = (unsigned long long)end_t_s * 1000 + end_t_ms;
//	auto& interval_t_ms = golden_config::interval_;
//	auto interval_t_s = interval_t_ms / 1000;
//	bool timestamp_is_second = (interval_t_ms % 1000 == 0) ? true : false;
//
//	int query_count = 1 + timestamp_is_second ? floor(double(start_t_s - end_t_s) / interval_t_s) : floor((double)(end_t - start_t) / interval_t_ms);
//
//	//�������
//	int batch_count = count;
//	vector<golden_int32> datetimes(batch_count);
//	vector<golden_int16> ms(batch_count, 0);
//	vector<golden_int16> quality(batch_count, GOLDEN_Q_GOOD);
//	vector<golden_error> errors(batch_count, 0);
//	//ֵ������������Բ�ͬ����ѡ��ͬ�Ļ�������
//	auto&& fvalues = g_vvfvalues;
//	auto&& ivalues = g_vvxvalues;
//	//������װ������д���ջ���д��ʷ
//	std::function<golden_error(golden_int32, golden_int32*, const golden_int32 *, const golden_int32 *, const golden_int16 *, const golden_float64 *, const golden_int64 *, const golden_int16 *, golden_error *)> put_datas = golden_config::get_history_data_ ? goh_put_archived_values : gos_put_snapshots;
//
//	golden_int64 lasttime_ms = (golden_int64)start_t_s * 1000;
//	golden_int64 endtime_ms = (golden_int64)end_t_s * 1000 + 999;
//	stop_watch api_watch;
//	auto sleep_until_time = std::chrono::milliseconds(golden_config::elapse_time_);  // ���Ƶȴ����
//	auto sleep_time_if_not_ready = std::chrono::seconds(5);
//	double elapsed_time = 0.0;              // �ܺ�ʱ
//	unsigned long long total_count = 0;     // ��������
//	int source_data_position = 0;           // ������Դ��ȡ���ݵ�λ�ã���һ��������ѭ��
//	golden_int32 put_count = (golden_int32)((endtime_ms - lasttime_ms + 1) / increment_t_ms);     // д�����
//	golden_int32 completed_count = 1;
//
//	while (lasttime_ms < endtime_ms)
//	{
//		auto&& abs_time = std::chrono::system_clock::now();
//		std::fill(datetimes.begin(), datetimes.end(), (golden_int32)(lasttime_ms / 1000));
//		if (!timestamp_is_second)
//			std::fill(ms.begin(), ms.end(), (golden_int16)(lasttime_ms % 1000));
//		std::fill(errors.begin(), errors.end(), (golden_error)0);
//
//		golden_int32 put_value_count = batch_count;
//	PUT_DATAS:
//		auto put_value_count_successful = put_value_count;
//		api_watch.restart();
//		ecode = put_datas(nHanle, &put_value_count_successful, g_ids.data() + pos, datetimes.data(), ms.data(),
//			fvalues.at(source_data_position).data() + pos, ivalues.at(source_data_position).data() + pos, quality.data(), errors.data());
//		if (ecode == GoE_ARV_PAGE_NOT_READY || ecode == GoE_ARVEX_PAGE_NOT_READY) {
//			g_logger->warn("The database cache is full, wait 5 seconds to try again.");
//			this_thread::sleep_for(sleep_time_if_not_ready);
//			goto PUT_DATAS;
//		}
//		api_watch.stop();
//		g_logger->info("Put datas batch: start_id={}, point_count={}, successful={}, timestamp={}ms, ret=0x{:x}, elapsed={}ms",
//			g_ids.at(pos), put_value_count, put_value_count_successful, lasttime_ms, ecode, api_watch.elapsed_ms());
//		CHECK_ECODE(ecode, fmt::format("Put datas, start_id={}, point_count={}, timestamp={}ms", g_ids.at(pos), put_value_count, lasttime_ms).c_str(), g_logger);
//		lasttime_ms += increment_t_ms;
//		source_data_position = (source_data_position + 1) % golden_config::func_period_;
//		elapsed_time += api_watch.elapsed_ms();
//		total_count += put_value_count;
//		if (show_progress) {
//			g_progress->critical("progress : {:.2f}%", (double)completed_count * 100 / put_count);
//			++completed_count;
//		}
//		abs_time += sleep_until_time;
//		if (abs_time > std::chrono::system_clock::now()) {
//			this_thread::sleep_until(abs_time);
//		}
//		if (ecode != GoE_OK) {
//			return ecode;
//		}
//	}
//	g_logger->warn("Put datas summary : start_id={}, point_count={}, put_count={}, total={}, elapsed={}ms, avg_elapsed={}ms, count_per_sec={}",
//		g_ids.at(pos), count, put_count, total_count, elapsed_time, elapsed_time / put_count, (double)(total_count * 1000) / elapsed_time);
//	all_thread_single_call_avg_elapsed += (elapsed_time / put_count);
//	g_query_total_count += total_count;
//
//	//�����б�ǩ��δд���Ĳ���ʷ����ҳд��浵�ļ���
//	//�����Ҫ�����鿴���ݣ������ڳ����˳�ʱ���øú�����
//	//������Ƶ�����ûή�����ݿ�����
//	int flush_count = 0;
//	if (golden_config::get_history_data_) {
//		for (int i = pos; i < pos + count; ++i)
//		{
//			ecode = goh_flush_archived_values(nHanle, g_ids.at(i), &flush_count);
//			g_logger->info("Flush datas, id={}, count={}", g_ids.at(i), flush_count);
//		}
//	}
//
//	return GoE_OK;
//}

// ��ѯ����
bool query_data()
{
	if (0 == golden_config::query_mode_.compare("corss_seccen")) {

	}
	else {
		// ��ѯ�̳߳�
		golden_config::thread_count_ = min(golden_config::point_count_, golden_config::thread_count_); // �߳������ܴ��ڱ�ǩ����
		int remaining_count = golden_config::point_count_ % golden_config::thread_count_;              // ���߳�������֮��ʣ��ĸ���
		int point_count_per_thread = golden_config::point_count_ / golden_config::thread_count_;       // ÿ���̸߳���ı�ǩ����
		g_thread_pool.init(golden_config::thread_count_);
		vector<std::future<golden_error> >results;
		int completed_count = 1;
		stop_watch parallel_thread_time;
		parallel_thread_time.start();
		if (0 == golden_config::query_mode_.compare("history_archived")) {
			for (int i = 0; i < golden_config::thread_count_; ++i) {
				results.emplace_back(g_thread_pool.enqueue(get_history_archived_thread, i * point_count_per_thread, ((remaining_count && (golden_config::thread_count_ - 1 == i)) ? remaining_count : point_count_per_thread)));
			}
		}
		else if (0 == golden_config::query_mode_.compare("history_archived_ex")) {
			for (int i = 0; i < golden_config::thread_count_; ++i) {
				results.emplace_back(g_thread_pool.enqueue(get_history_archived_ex_thread, i * point_count_per_thread, ((remaining_count && (golden_config::thread_count_ - 1 == i)) ? remaining_count : point_count_per_thread)));
			}
		}
		else if (0 == golden_config::query_mode_.compare("plot_value")) {
			for (int i = 0; i < golden_config::thread_count_; ++i) {
				results.emplace_back(g_thread_pool.enqueue(get_plot_value_thread, i * point_count_per_thread, ((remaining_count && (golden_config::thread_count_ - 1 == i)) ? remaining_count : point_count_per_thread)));
			}
		}
		else if (0 == golden_config::query_mode_.compare("interval_value")) {
			for (int i = 0; i < golden_config::thread_count_; ++i) {
				results.emplace_back(g_thread_pool.enqueue(get_interval_value_thread, i * point_count_per_thread, ((remaining_count && (golden_config::thread_count_ - 1 == i)) ? remaining_count : point_count_per_thread)));
			}
		}
		else if(0 == golden_config::query_mode_.compare("summary_value")) {
			for (int i = 0; i < golden_config::thread_count_; ++i) {
				results.emplace_back(g_thread_pool.enqueue(get_summary_value_thread, i * point_count_per_thread, ((remaining_count && (golden_config::thread_count_ - 1 == i)) ? remaining_count : point_count_per_thread)));
			}
		}
		else if (0 == golden_config::query_mode_.compare("section_summary_value")) {
			for (int i = 0; i < golden_config::thread_count_; ++i) {
				results.emplace_back(g_thread_pool.enqueue(get_section_summary_value_thread, i * point_count_per_thread, ((remaining_count && (golden_config::thread_count_ - 1 == i)) ? remaining_count : point_count_per_thread)));
			}
		}
		else {
			for (int i = 0; i < golden_config::thread_count_; ++i) {
				results.emplace_back(g_thread_pool.enqueue(get_snapshot_value_thread, i * point_count_per_thread, ((remaining_count && (golden_config::thread_count_ - 1 == i)) ? remaining_count : point_count_per_thread)));
			}
		}

		for (auto&& result : results) {
			result.get();
			g_progress->warn("progress : {:.2f}%", (double)completed_count * 100 / golden_config::point_count_);
			++completed_count;
			if (!g_running) break;
		}
		parallel_thread_time.stop();
		g_thread_pool.close();
		g_logger->critical("The time to query all data is {:.2f}ms, accumulated time of all calls is {:.2f}ms, and total value count is {}.", parallel_thread_time.elapsed_ms(), all_thread_all_call_total_elapsed, g_query_total_count);
		if (!golden_config::result_file_.empty()) {
			try {
				spdlog::details::file_helper file_helper;
				file_helper.open(golden_config::result_file_);
				fmt::memory_buffer buffer;
				fmt::writer writer(buffer);
				auto fmt_specs = fmt::basic_format_specs<char>();
				fmt_specs.precision_ = -3;  // ����������3λС��
				auto *name_ptr = golden_config::task_name_.data();
				buffer.append(name_ptr, name_ptr + golden_config::task_name_.size());
				buffer.push_back(',');
				writer.write(parallel_thread_time.elapsed_ms(), fmt_specs);
				buffer.push_back(',');
				writer.write(all_thread_all_call_total_elapsed, fmt_specs);
				buffer.push_back(',');
				writer.write(g_query_total_count);
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
	}
	return true;
}

int main(int argc, char *argv[])
{
	std::cout << ",,--.                    .-,--.      .      " << endl;
	std::cout << "|`. | . . ,-. ,-. . .    ' |   \\ ,-. |- ,-. " << endl;
	std::cout << "|  .| | | |-' |   | |    , |   / ,-| |  ,-| " << endl;
	std::cout << "`---\\ `-^ `-' '   `-|    `-^--'  `-^ `' `-^ " << endl;
	std::cout << "     `             /|                       " << endl;
	std::cout << "                  `-'                       " << endl << endl;
	// http://patorjk.com/software/taag/#p=display&f=Stampatello&t=Golden%20Query%20Data

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
		app.add_option("-s,--starttime", golden_config::start_time_, "start time (=now)\nformat:\n \"YYYY-MM-DD hh:mm:ss.ms\"\n \"mintime\" start time is min UTC time : 1970-01-01 00:00:00.000\n \"now\" start time is local real time\n Enclosed in single or double quotes.");
		golden_config::end_time_ = "forever";
		app.add_option("-e,--endtime", golden_config::end_time_, "end time (=forever)\nformat:\n \"YYYY-MM-DD hh:mm:ss.ms\"\n \"forever\" end time is max UTC time");
#ifdef _WIN32
		app.add_option("--search", golden_config::search_condition_, "search condition");
#endif //_WIN32
		golden_config::first_point_ = 1;
		app.add_option("--first_point", golden_config::first_point_, "first point's id (=1)");
		golden_config::point_count_ = 1;
		app.add_option("--point_count", golden_config::point_count_, "point count (=1)");
		golden_config::point_interval_ = 1;
		app.add_option("--point_interval", golden_config::point_interval_, "point interval (=1)");
		golden_config::query_mode_ = "history_archived";
		app.add_option("--query_mode", golden_config::query_mode_, "query mode (=history_archived)\n 1.history_archived\n 2.history_archived_ex\n 3.plot_value\n 4.interval_value\n 5.summary_value\n 6.section_summary_value\n 7.snapshot_value");
		golden_config::query_batch_count_ = 1000;
		app.add_option("--query_batch_count", golden_config::query_batch_count_, "query values count per batch (=1000)");
		golden_config::interval_ = 1000;
		app.add_option("--interval", golden_config::interval_, "query interval (=1000)");
		golden_config::thread_count_ = 1;
		app.add_option("--thread_count", golden_config::thread_count_, "thread count (=1)");
		golden_config::thread1_count_ = 1;
		app.add_option("--thread1_count", golden_config::thread1_count_, "thread1 count (=1) section_summary_value use");
		golden_config::print_log_ = true;
		app.add_flag("--print_log", golden_config::print_log_, "print log to console");
		golden_config::log_level_ = spdlog::level::level_enum::info;
		app.add_option("--log_level", golden_config::log_level_, "log level (=2) as info\n 0.trace\n 1.debug\n 2.info\n 3.warn\n 4.err\n 5.critical\n 6.off");
		app.add_option("--result_file", golden_config::result_file_, "result file path, default is empty.");
#ifdef _WIN32
		app.add_flag("--Attention", "# ע�⣺\n#   1.���Դ���������ʱ�䷶Χ\n#   2.�����ѯ�����ǩ�㣬���Զ����䵽����̲߳�ѯ");
#elif _LINUX
		app.add_flag("--Attention", "Attention:\n1.Time range with milliseconds can be passed in.\n2.If multiple points are queried, they are automatically assigned to multiple query threads.");
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

	g_logger->trace("Querying data.");
	if (!query_data())
		return -1;

	g_logger->info("Exit the app.");
	return 0;
}