#ifndef _SPP_COMM_SERVERBASE_H_
#define _SPP_COMM_SERVERBASE_H_

#include "cprocmon.h"


#define SIG_FLAG_QUIT 0x01

/**
继承关系的初始化顺序：
1. 父类数据成员初始化；
2. 父类构造函数；
3. 子类数据成员初始化；
4. 子类构造函数；
**/

class CServerBase
{
public:
	CServerBase();
	~CServerBase();
 
	void run();
	virtual void startup();
	virtual void realrun();

	static void sigusr1_handle(int signo);
	static void sigusr2_handle(int signo);
	static bool isquit();
	static int sigflag;

protected:
	CProcMonitorCli procmonCli;
};

#endif