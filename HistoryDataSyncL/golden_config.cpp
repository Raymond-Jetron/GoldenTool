#include "golden_config.h"

// 外部传入
std::string golden_config::task_name_;
std::string golden_config::source_host_name_;
int golden_config::source_port_;
std::string golden_config::source_user_;
std::string golden_config::source_password_;
std::string golden_config::sink_host_name_;
int golden_config::sink_port_;
std::string golden_config::sink_user_;
std::string golden_config::sink_password_;
std::string golden_config::start_time_;
std::string golden_config::end_time_;
int golden_config::thread_count_;
std::string golden_config::points_dir_;
bool golden_config::output_points_prop_;
int golden_config::log_level_;
bool golden_config::print_log_;

// 内部转换
int golden_config::start_time_int_;
short golden_config::start_time_ms_;
int golden_config::end_time_int_;
short golden_config::end_time_ms_;
