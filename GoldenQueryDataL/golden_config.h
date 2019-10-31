#pragma once
#include <string>
#include <vector>

class golden_config
{
public:
	golden_config() {};
	~golden_config() {};

	static std::string task_name_;
	static std::vector<std::string> host_name_;
	static int port_;
	static std::string user_;
	static std::string password_;
	static std::string start_time_;
	static std::string end_time_;
	static std::string search_condition_;
	static int first_point_;
	static int point_count_;
	static int point_interval_;
	static std::string query_mode_;
	static int query_batch_count_;
	static int interval_;
	static int thread_count_;
	static bool print_log_;
	static int log_level_;
	static std::string result_file_;

	static int start_time_int_;
	static short start_time_ms_;
	static int end_time_int_;
	static short end_time_ms_;
};


