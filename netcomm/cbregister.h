#ifndef _CB_REGISTER_H_
#define _CB_REGISTER_H_

#include <map>

using namespace std;

#define CB_RECV_CLIENTDATA 1

typedef int (*cb_func_t)(int flag, void *arg1, void *parms);

class NetcommCallback
{
public:
	NetcommCallback();
	NetcommCallback(cb_func_t cb_func, void *parms);
	cb_func_t cb_func;
	void *parms;
};

/**
功能：接收上层函数注册，下层执行回调，将结果上报到上层处理。
**/
class NetcommCallbackRegister
{
public:
	NetcommCallbackRegister();
	void registCallback(int id, cb_func_t func, void *parms);
	NetcommCallback& getCallback(int id);
private:
	map<int, NetcommCallback> funclist;
};


#endif