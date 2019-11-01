#include "process_utility.h"

#ifdef _WIN32
#include <windows.h>
#include <TlHelp32.h>
#pragma comment(lib, "user32.lib")

namespace process_utility
{
	bool find_process_id_array(vector<process_data>& process_array)
	{
		if (process_array.empty())
			return false;

		HANDLE  hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
		if (hProcessSnap == INVALID_HANDLE_VALUE)
			return false;
		bool _exist = false;
		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32);
		for (vector<process_data>::iterator iter = process_array.begin(); iter != process_array.end(); ++iter)
		{
			process_data& _data = *iter;
			BOOL bMore = Process32First(hProcessSnap, &pe32);
			do
			{
				if (_stricmp(pe32.szExeFile, _data.process_name.c_str()) == 0)
				{
					_data.process_id = pe32.th32ProcessID;
					_exist = true;
					break;
				}
			} while (Process32Next(hProcessSnap, &pe32));
		}
		return _exist;
	}

	bool find_process_thread_id(int process_id, int& thread_id)
	{
		if (process_id <= 0)
			return false;

		HANDLE  hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, process_id);
		if (hModuleSnap == INVALID_HANDLE_VALUE)
			return false;
		THREADENTRY32 te32;
		te32.dwSize = sizeof(THREADENTRY32);
		BOOL bMore = Thread32First(hModuleSnap, &te32);
		do
		{
			if (te32.th32OwnerProcessID == process_id)
			{
				thread_id = te32.th32ThreadID;
				return true;
			}
		} while (Thread32Next(hModuleSnap, &te32));
		CloseHandle(hModuleSnap);
		return false;
	}

	bool is_process_exit(int process_id)
	{
		if (process_id <= 0)
			return true;

		HANDLE  hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hModuleSnap == INVALID_HANDLE_VALUE)
			return true;
		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32);
		BOOL bMore = Process32First(hModuleSnap, &pe32);
		do
		{
			if (pe32.th32ProcessID == process_id)
			{
				return false;
			}
		} while (Process32Next(hModuleSnap, &pe32));
		CloseHandle(hModuleSnap);
		return true;
	}
	
	bool post_thread_exist(int thread_id)
	{
		return (PostThreadMessage(thread_id, WM_QUIT, 0, 0) ? true : false);
	}

	unsigned long get_last_error() {
		return GetLastError();
	}
}

#else // _LINUX
namespace process_utility
{
	bool find_process_id_array(vector<process_data>& process_array)
	{
		if (process_array.empty())
			return false;

		bool _exist = false;

		return _exist;
	}

	bool find_process_thread_id(int process_id, int& thread_id)
	{
		if (process_id <= 0)
			return false;

		return false;
	}

	bool is_process_exit(int process_id)
	{
		if (process_id <= 0)
			return true;

		return true;
	}

	bool post_thread_exist(int thread_id)
	{
		return false;
	}

	unsigned long get_last_error() { return 0; }
}

#endif