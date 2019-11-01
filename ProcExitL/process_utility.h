#pragma once
#ifdef _WIN32
#include <windows.h>
#include <TlHelp32.h>
#pragma comment(lib, "user32.lib")
#else // _LINUX

#endif //_WIN32

#include <string>
#include <vector>
using std::string;
using std::vector;

typedef struct _process_data
{
	int process_id;
	string process_name;
} process_data;

namespace process_utility
{
	bool find_process_id_array(vector<process_data>& process_array);

	bool find_process_thread_id(int process_id, int& thread_id);

	bool is_process_exit(int process_id);

	bool post_thread_exist(int thread_id);

	unsigned long get_last_error();
}
