#pragma once

#include <string>
#include <iostream>
#include "goldenapi.h"

namespace golden
{
	golden_error golden_verify(const char *statement, golden_error error, golden_error exception, const char *file, int line)
	{
		if (error != GoE_OK && error != exception)
		{
			const int size = 256;
			char error_name[size] = { 0 };
			char error_msg[size] = { 0 };
			go_format_message(error, error_msg, error_name, size);

			std::string apiname(statement);
			apiname = apiname.substr(apiname.find_first_not_of(' '), apiname.find_first_not_of(' ') + apiname.find_first_not_of('('));
			std::cout << "FILE:" << file << ",LINE:" << line << "," << apiname.c_str() << "µ÷ÓÃÊ§°Ü," << "´íÎóÂë[" << error_name << " 0x" << std::hex << error << "]:" << error_msg << std::endl;
		}
		return error;
	}
}

#define GOLDEN_VERIFY(ecode, statement, apimutex) \
{ \
	std::unique_lock<std::mutex> lock(apimutex); \
	ecode = statement; \
	golden::golden_verify(statement, ecode, GoE_OK, __FILE__, __LINE__); \
}