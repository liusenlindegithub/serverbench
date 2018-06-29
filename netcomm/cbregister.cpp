#include <stdlib.h>
#include <utility>

#include "cbregister.h"

using namespace std;

NetcommCallback::NetcommCallback()
{
	cb_func = NULL;
	parms = NULL;
}

NetcommCallback::NetcommCallback(cb_func_t cb_func, void *parms)
{
	this->cb_func = cb_func;
	this->parms = parms;
}

NetcommCallbackRegister::NetcommCallbackRegister()
{}

void NetcommCallbackRegister::registCallback(int id, cb_func_t func, void *parms)
{
	NetcommCallback callback(func, parms);
	funclist.insert(make_pair(id, callback));
}

NetcommCallback& NetcommCallbackRegister::getCallback(int id)
{
	NetcommCallback callback;
	map<int, NetcommCallback>::iterator itor = funclist.find(id);
	if(itor != funclist.end())
	{
		return itor->second;
	}

	return callback;
}