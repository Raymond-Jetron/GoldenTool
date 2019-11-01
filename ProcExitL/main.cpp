#include <cstdio>
#include <iostream>
#include <thread>
#include "process_utility.h"
#include "spdlog/spdlog.h"
#include "CLI/CLI.hpp"

int main(int argc, char* argv[])
{
	std::vector<int> proc_ids;
	std::vector<std::string> proc_names;

	auto _wait_process_exit_func = [&](const int& _process_id)
	{
		while (true)
		{
			if (!process_utility::is_process_exit(_process_id))
			{
				std::this_thread::sleep_for(std::chrono::seconds(100));
			}
			else break;
		}
		std::cout << "进程已经退出。" << std::endl;
	};

	CLI::App app{ "App description" };
	{
		app.add_option("-i,--id", proc_ids, "process ids \n Exit process by pid.\n e.g. -i 1000 1001 1002");
		app.add_option("-n,--name", proc_names, "process names \n Exit process by name.\n e.g. -n process0.exe process1.exe process2.exe");
	}
	try {
		(app).parse((argc), (argv));
		if (!proc_ids.empty()) {
			int thread_id = 0;
			for (auto proc_id : proc_ids) {
				if (!process_utility::find_process_thread_id(proc_id, thread_id))
				{
					std::cout << "Not find the process, pid is : " << proc_id << std::endl;
					continue;
				}
				if (!process_utility::post_thread_exist(thread_id))
				{
					auto err_code = process_utility::get_last_error();
					std::cout << "要求进程退出操作失败，进程编号：" << proc_id << "，线程编号：" << thread_id << "，错误码：" << err_code << std::endl;
					continue;
				}
				std::cout << "正在等待进程退出，进程编号：" << proc_id << std::endl;
				//等待退出
				_wait_process_exit_func(proc_id);
			}
		}
		else if (!proc_names.empty()) {
			vector<process_data> process_array;
			for (auto& pname : proc_names)
			{
				process_array.push_back(process_data{ 0, pname });
			}
			if (!process_utility::find_process_id_array(process_array))
			{
				std::cout << "没有找到符合条件的进程信息。" << std::endl;
				return 0;
			}
			int thread_id = 0;
			for (auto& process_data : process_array)
			{
				if (process_data.process_id == 0)
				{
					std::cout << "没有找到符合条件的进程信息。进程名称：" << process_data.process_name << std::endl;
					continue;
				}
				if (!process_utility::find_process_thread_id(process_data.process_id, thread_id))
				{
					std::cout << "没有找到进程的主线程。进程编号：" << process_data.process_id << std::endl;
					continue;
				}
				if (!process_utility::post_thread_exist(thread_id))
				{
					auto err_code = process_utility::get_last_error();
					std::cout << "要求进程退出操作失败，进程名称：" << process_data.process_name << "，线程编号：" << thread_id << "，错误码：" << err_code << std::endl;
					continue;
				}
				std::cout << "正在等待进程退出，进程名称：" << process_data.process_name << std::endl;
				//等待退出
				_wait_process_exit_func(process_data.process_id);
			}
			std::cout << "向进程发送消息操作完成。" << std::endl;
		}
		else {
			throw CLI::CallForHelp();
		}
		return 0;
	}
	catch (const CLI::ParseError &e) {
		return (app).exit(e);
	}
}