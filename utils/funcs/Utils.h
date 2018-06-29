#ifndef _UTILS_H_
#define _UTILS_H_

class Utils
{
public:
	// 命令行执行
	static int mypopen(char *cmd,char type);
	// 字符串处理：去掉结尾回车换行
	static void delrn(char *str);
};

#endif