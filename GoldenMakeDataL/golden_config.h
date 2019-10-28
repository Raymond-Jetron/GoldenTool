#pragma once
#include <string>
#include <vector>

class golden_config
{
public:
	golden_config() {};
	~golden_config() {};
	//外部传入参数
	static std::string task_name_;
	static std::vector<std::string> host_name_;
	static int port_;
	static std::string user_;
	static std::string password_;
	static std::string start_time_;
	static std::string end_time_;
	static int elapse_time_;
	static int increment_time_;
	static bool put_history_data_;
	static int min_value_;
	static int max_value_;
	static std::string generator_;
	static std::string search_condition_;
	static int first_point_;
	static int point_count_;
	static int point_interval_;
	static std::string write_mode_;
	static int threadcount_;
	static bool print_log_;
	static int log_level_;
	static int func_period_;
	static std::string result_file_;
	
	//内部使用参数
	static int start_time_int_;
	static int end_time_int_;
};


