#ifndef _LOG_H_
#define _LOG_H_

#define LOG_LEVEL_ERROR   1
#define LOG_LEVEL_WARNING 2
#define LOG_LEVEL_INFO    3
#define LOG_LEVEL_DEBUG   4

#define LOG_MSG_SIZE 102400

#include <stdarg.h>

#include <string>
using namespace std;

#define log_error(fmt, args...) Log::i_log_error(__FILE__, __FUNCTION__, __LINE__, fmt, ##args)
#define log_warn(fmt, args...)  Log::i_log_warn(__FILE__, __FUNCTION__, __LINE__, fmt, ##args)
#define log_info(fmt, args...)  Log::i_log_info(__FILE__, __FUNCTION__, __LINE__, fmt, ##args)
#define log_debug(fmt, args...) Log::i_log_debug(__FILE__, __FUNCTION__, __LINE__, fmt, ##args)
 
class Log
{
public:
	static void i_log_error(const char *code_file, const char *code_func, int code_line, const char *fmt, ...);
	static void i_log_warn(const char *code_file, const char *code_func, int code_line, const char *fmt, ...);
	static void i_log_info(const char *code_file, const char *code_func, int code_line, const char *fmt, ...);
	static void i_log_debug(const char *code_file, const char *code_func, int code_line, const char *fmt, ...);

private:
	static int curr_log_level;
	static string logdir;
	static string appname;
	static void write_log(int level, const char *code_file, const char *code_func, int code_line, const char *format, va_list args);
};

#endif