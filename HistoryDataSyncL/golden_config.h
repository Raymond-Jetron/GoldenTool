#pragma once
#include <string>

class golden_config
{
public:
	golden_config() {};
	~golden_config() {};

	static std::string task_name_;
	static std::string source_host_name_;
	static int source_port_;
	static std::string source_user_;
	static std::string source_password_;
	static std::string sink_host_name_;
	static int sink_port_;
	static std::string sink_user_;
	static std::string sink_password_;
	static std::string start_time_;
	static std::string end_time_;
	static int thread_count_;
	static std::string points_dir_;
	static bool output_points_prop_;
	static int log_level_;
	static bool print_log_;

	static int start_time_int_;
	static short start_time_ms_;
	static int end_time_int_;
	static short end_time_ms_;
};
