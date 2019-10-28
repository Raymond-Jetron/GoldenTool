#pragma once
#ifndef __COMMON_H__
#define __COMMON_H__

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#elif _LINUX
#include <stdarg.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#endif

#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define ACCESS _access
#define MKDIR(a) _mkdir((a))
#define STRDUP(d) _strdup(d)
#define PATH_SEP '\\'
#elif _LINUX
#define ACCESS access
#define MKDIR(a) mkdir((a),0755)
#define STRDUP(d) strdup(d)
#define PATH_SEP '/'
#endif


int creat_dir(const char *pDir)
{
	int i = 0;
	int iRet;
	int iLen;
	char* pszDir;

	if (NULL == pDir)
	{
		return 0;
	}

	//如果存在,返回
	iRet = ACCESS(pDir, 0);
	if (iRet == 0)
		return 0;

	pszDir = STRDUP(pDir);
	iLen = (int)strlen(pszDir);

	// 创建中间目录
	for (i = 1; i < iLen; i++)
	{
		if (pszDir[i] == '\\' || pszDir[i] == '/')
		{
			pszDir[i] = '\0';

			//如果不存在,创建
			iRet = ACCESS(pszDir, 0);
			if (iRet != 0)
			{
				iRet = MKDIR(pszDir);
				if (iRet != 0)
				{
					return -1;
				}
			}
			//支持linux,将所有\换成/
			pszDir[i] = PATH_SEP;
		}
	}

	iRet = MKDIR(pszDir);
	free(pszDir);
	return iRet;
}

#ifdef _WIN32
#elif _LINUX

constexpr auto VMRSS_LINE = 17;
constexpr auto VMSIZE_LINE = 13;
constexpr auto PROCESS_ITEM = 14;

namespace systemperf {
typedef struct {
	unsigned long user;
	unsigned long nice;
	unsigned long system;
	unsigned long idle;
}Total_Cpu_Occupy_t;


typedef struct {
	unsigned int pid;
	unsigned long utime;  //user time
	unsigned long stime;  //kernel time
	unsigned long cutime; //all user time
	unsigned long cstime; //all dead time
}Proc_Cpu_Occupy_t;


//获取第N项开始的指针
const char* get_items(const char*buffer, int item) {

	const char *p = buffer;

	int len = (int)strlen(buffer);
	int count = 0;

	for (int i = 0; i < len; i++) {
		if (' ' == *p) {
			count++;
			if (count == item - 1) {
				p++;
				break;
			}
		}
		p++;
	}

	return p;
}


//获取总的CPU时间
unsigned long get_cpu_total_occupy() {

	FILE *fd;
	char buff[1024] = { 0 };
	Total_Cpu_Occupy_t t;

	fd = fopen("/proc/stat", "r");
	if (nullptr == fd) {
		return 0;
	}

	fgets(buff, sizeof(buff), fd);
	char name[64] = { 0 };
	sscanf(buff, "%s %ld %ld %ld %ld", name, &t.user, &t.nice, &t.system, &t.idle);
	fclose(fd);

	return (t.user + t.nice + t.system + t.idle);
}


//获取进程的CPU时间
unsigned long get_cpu_proc_occupy(unsigned int pid) {

	char file_name[64] = { 0 };
	Proc_Cpu_Occupy_t t;
	FILE *fd;
	char line_buff[1024] = { 0 };
	sprintf(file_name, "/proc/%d/stat", pid);

	fd = fopen(file_name, "r");
	if (nullptr == fd) {
		return 0;
	}

	fgets(line_buff, sizeof(line_buff), fd);

	sscanf(line_buff, "%u", &t.pid);
	const char *q = get_items(line_buff, PROCESS_ITEM);
	sscanf(q, "%ld %ld %ld %ld", &t.utime, &t.stime, &t.cutime, &t.cstime);
	fclose(fd);

	return (t.utime + t.stime + t.cutime + t.cstime);
}


//获取CPU占用率
float get_proc_cpu(unsigned int pid) {

	unsigned long totalcputime1, totalcputime2;
	unsigned long procputime1, procputime2;

	totalcputime1 = get_cpu_total_occupy();
	procputime1 = get_cpu_proc_occupy(pid);

	usleep(200000);

	totalcputime2 = get_cpu_total_occupy();
	procputime2 = get_cpu_proc_occupy(pid);

	float pcpu = 0.0;
	if (0 != totalcputime2 - totalcputime1) {
		pcpu = (float)(100.0 * (double)(procputime2 - procputime1) / (double)(totalcputime2 - totalcputime1));
	}

	return pcpu;
}


//获取进程占用内存
unsigned int get_proc_mem(unsigned int pid) {

	char file_name[64] = { 0 };
	FILE *fd;
	char line_buff[512] = { 0 };
	sprintf(file_name, "/proc/%d/status", pid);

	fd = fopen(file_name, "r");
	if (nullptr == fd) {
		return 0;
	}

	char name[64];
	int vmrss;
	for (int i = 0; i < VMRSS_LINE - 1; i++) {
		fgets(line_buff, sizeof(line_buff), fd);
	}

	fgets(line_buff, sizeof(line_buff), fd);
	sscanf(line_buff, "%s %d", name, &vmrss);
	fclose(fd);

	return vmrss;
}


//获取进程占用虚拟内存
unsigned int get_proc_virtualmem(unsigned int pid) {

	char file_name[64] = { 0 };
	FILE *fd;
	char line_buff[512] = { 0 };
	sprintf(file_name, "/proc/%d/status", pid);

	fd = fopen(file_name, "r");
	if (nullptr == fd) {
		return 0;
	}

	char name[64];
	int vmsize;
	for (int i = 0; i < VMSIZE_LINE - 1; i++) {
		fgets(line_buff, sizeof(line_buff), fd);
	}

	fgets(line_buff, sizeof(line_buff), fd);
	sscanf(line_buff, "%s %d", name, &vmsize);
	fclose(fd);

	return vmsize;
}


//进程本身
int get_pid(const char* process_name, const char* user = nullptr)
{
	if (user == nullptr) {
		user = getlogin();
	}

	char cmd[512];
	if (user) {
		sprintf(cmd, "pgrep %s -u %s", process_name, user);
	}

	FILE *pstr = popen(cmd, "r");

	if (pstr == nullptr) {
		return 0;
	}

	char buff[512];
	::memset(buff, 0, sizeof(buff));
	if (NULL == fgets(buff, 512, pstr)) {
		return 0;
	}

	return atoi(buff);
}

};
#endif

#include <chrono>
class stop_watch 
{
public:
	stop_watch(){}

	void start()
	{
		if (!running)
		{
			begin_time_ = std::chrono::steady_clock::now();
			running = true;
		}
	}

	void stop()
	{
		if (running)
		{
			end_time_ = std::chrono::steady_clock::now();
			elapsed_ = end_time_ - begin_time_;
			running = false;
		}
	}

	void restart()
	{
		elapsed_ = std::chrono::steady_clock::duration::zero();
		begin_time_ = std::chrono::steady_clock::now();
		running = true;
	}

	//秒
	double elapsed_second()
	{
		return ((double)std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_).count() / 1e3);
	}
	//毫秒
	double elapsed_ms()
	{
		return ((double)std::chrono::duration_cast<std::chrono::microseconds>(elapsed_).count() / 1e3);
	}

protected:
	std::chrono::steady_clock::duration elapsed_ = std::chrono::steady_clock::duration::zero();
	std::chrono::steady_clock::time_point begin_time_;
	std::chrono::steady_clock::time_point end_time_;
	bool running = false;
};

#include <time.h>
//由时间值转换为字符串（字符串格式：%4d-%2d-%2d %2d:%2d:%2d.%3.3d)
std::string cast_time_ms_str(const time_t timeval, short ms)
{
	tm* local = localtime(&timeval);
	local->tm_year += 1900;
	local->tm_mon++;

	char szTime[32] = { 0 };
	//strftime(szTime, 32, "%Y-%m-%d %H:%M:%S", local);
	sprintf(szTime, "%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d.%3.3d",
		local->tm_year,
		local->tm_mon,
		local->tm_mday,
		local->tm_hour,
		local->tm_min,
		local->tm_sec,
		ms);
	return szTime;
}
//由时间值转换为字符串（字符串格式：%4d%2d%2d%2d%2d%2d)
std::string cast_time_str_simple(const time_t timeval)
{
	tm* local = localtime(&timeval);
	local->tm_year += 1900;
	local->tm_mon++;

	char szTime[32] = { 0 };
	sprintf(szTime, "%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",
		local->tm_year,
		local->tm_mon,
		local->tm_mday,
		local->tm_hour,
		local->tm_min,
		local->tm_sec);
	return szTime;
}

std::string trans_classof(unsigned int classof)
{
	switch (classof & Golden_Mark_Classof)
	{
	case GOLDEN_BASE:					return "base point";
	case GOLDEN_SCAN:					return "scan point";
	case GOLDEN_CALC:					return "calc point";
	case GOLDEN_SCAN | GOLDEN_CALC:	    return "scan&calc point";
	default:                            return "no support";
	}
};

#include <fstream>
class output_file
{
public:
	output_file()
	{
		output_file();
	}
	output_file(const std::string& file_path, bool output)
	{
		if (output) {
			if (ofs_.is_open())    ofs_.close();
			ofs_.open(file_path, std::fstream::trunc);
			//cout << file_path << endl;
		}
	}
	~output_file()
	{
		if (ofs_.is_open())
		{
			ofs_.close();
		}
	}

public:
	std::ofstream& ofs() { return ofs_; }

private:
	std::ofstream ofs_;
};

class input_file
{
public:
	input_file(const std::string& file_path)
	{
		if (ifs_.is_open())    ifs_.close();
		ifs_.open(file_path);
		//cout << file_path << endl;
	}
	~input_file()
	{
		if (ifs_.is_open())
		{
			ifs_.close();
		}
	}

public:
	std::ifstream& ifs() { return ifs_; }

private:
	std::ifstream ifs_;
};

#endif // !__COMMON_H__


