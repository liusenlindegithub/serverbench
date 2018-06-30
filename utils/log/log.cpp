#include <time.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include <iostream>
#include <fstream>
using namespace std;

#include "log.h"

int Log::curr_log_level = LOG_LEVEL_DEBUG;

string Log::logdir = "log";
string Log::appname = "default";

/**
	��ȡ�ɱ�����е�һ��������ַ������ʽ��д�롣
	* typedef char * va_list;
	* INTSIZEOF ��,��ȡ����ռ�õĿռ䳤�ȣ���Сռ�ó���Ϊint����������
	#define _INTSIZEOF(n) ( (sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1) )
	* VA_START�꣬��ȡ�ɱ�����б�ĵ�һ�������ĵ�ַ��ap������Ϊva_list��ָ�룬v�ǿɱ��������ߵĲ�������
	#define va_start(ap,v) ( ap = (va_list)&v + _INTSIZEOF(v) )
	* VA_ARG�꣬��ȡ�ɱ�����ĵ�ǰ����������ָ�����Ͳ���ָ��ָ����һ������t���������˵�ǰ���������ͣ���
	#define va_arg(ap,t) ( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )
	* VA_END�꣬���va_list�ɱ�����б�
	#define va_end(ap) ( ap = (va_list)0 )
**/
void Log::i_log_error(const char *code_file, const char *code_func, int code_line, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	if(curr_log_level >= LOG_LEVEL_ERROR)
	{
		write_log(LOG_LEVEL_ERROR, code_file, code_func, code_line, fmt, args);
	}

	va_end(args);
}

void Log::i_log_warn(const char *code_file, const char *code_func, int code_line, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	if(curr_log_level >= LOG_LEVEL_WARNING)
	{
		write_log(LOG_LEVEL_WARNING, code_file, code_func, code_line, fmt, args);
	}

	va_end(args);
}

void Log::i_log_info(const char *code_file, const char *code_func, int code_line, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	if(curr_log_level >= LOG_LEVEL_INFO)
	{
		write_log(LOG_LEVEL_INFO, code_file, code_func, code_line, fmt, args);
	}

	va_end(args);
}

void Log::i_log_debug(const char *code_file, const char *code_func, int code_line, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	if(curr_log_level >= LOG_LEVEL_DEBUG)
	{
		write_log(LOG_LEVEL_DEBUG, code_file, code_func, code_line, fmt, args);
	}

	va_end(args);
}

void Log::write_log(int level, const char *code_file, const char *code_func, int code_line, const char *format, va_list args)
{
	char buff[LOG_MSG_SIZE];
	char logfile[256];
	int buff_offset = 0;
	int _errno = errno;

	// ��ȡ��ǰʱ��
	struct tm tm;
	time_t now = time(NULL);
	localtime_r(&now, &tm);

	// ��ȡ��־�����ַ���
	char loglevel[128];
	switch(level)
	{
	case LOG_LEVEL_ERROR:
		strcpy(loglevel, "ERROR");
		break;
	case LOG_LEVEL_WARNING:
		strcpy(loglevel, "WARN");
		break;
	case LOG_LEVEL_INFO:
		strcpy(loglevel, "INFO");
		break;
	case LOG_LEVEL_DEBUG:
		strcpy(loglevel, "DEBUG");
		break;
	}
	
	buff_offset = snprintf(buff, LOG_MSG_SIZE, "<%s>[%02d:%02d:%02d] pid[%d] [%s::%s():%d] : ", 
		loglevel, tm.tm_hour, tm.tm_min, tm.tm_sec, getpid(), code_file, code_func, code_line);

	
	buff_offset += vsnprintf(buff + buff_offset, LOG_MSG_SIZE - buff_offset, format, args);

	buff[buff_offset++] = '\n';
	buff[buff_offset++] = '\0';
	
	int offset = snprintf(logfile, sizeof(logfile), "%s/%s_%04d%02d%02d.log", logdir.c_str(), appname.c_str(), tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
	logfile[offset] = '\0';
	ofstream fout(logfile, ios::app);
	fout << buff;
	fout.close();

	errno = _errno;
}

void Log::set_log_dir(string dirname, string appname)
{
	Log::logdir = dirname;
	Log::appname = appname;
}