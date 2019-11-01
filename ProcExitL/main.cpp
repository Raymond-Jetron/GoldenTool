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
		std::cout << "�����Ѿ��˳���" << std::endl;
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
					std::cout << "Ҫ������˳�����ʧ�ܣ����̱�ţ�" << proc_id << "���̱߳�ţ�" << thread_id << "�������룺" << err_code << std::endl;
					continue;
				}
				std::cout << "���ڵȴ������˳������̱�ţ�" << proc_id << std::endl;
				//�ȴ��˳�
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
				std::cout << "û���ҵ����������Ľ�����Ϣ��" << std::endl;
				return 0;
			}
			int thread_id = 0;
			for (auto& process_data : process_array)
			{
				if (process_data.process_id == 0)
				{
					std::cout << "û���ҵ����������Ľ�����Ϣ���������ƣ�" << process_data.process_name << std::endl;
					continue;
				}
				if (!process_utility::find_process_thread_id(process_data.process_id, thread_id))
				{
					std::cout << "û���ҵ����̵����̡߳����̱�ţ�" << process_data.process_id << std::endl;
					continue;
				}
				if (!process_utility::post_thread_exist(thread_id))
				{
					auto err_code = process_utility::get_last_error();
					std::cout << "Ҫ������˳�����ʧ�ܣ��������ƣ�" << process_data.process_name << "���̱߳�ţ�" << thread_id << "�������룺" << err_code << std::endl;
					continue;
				}
				std::cout << "���ڵȴ������˳����������ƣ�" << process_data.process_name << std::endl;
				//�ȴ��˳�
				_wait_process_exit_func(process_data.process_id);
			}
			std::cout << "����̷�����Ϣ������ɡ�" << std::endl;
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