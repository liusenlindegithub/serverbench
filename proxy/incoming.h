#ifndef _INCOMING_H_
#define _INCOMING_H_

#include <memory>
using namespace std;

#include "../netcomm/pollerunit.h"
#include "../comm/cprocmon.h"
#include "../comm/serverbase.h"

typedef struct{
	int socket_type;
	int port;
}socket_conf;

/**
功能：
监听连接，解析消息。
（1）创建epoll
（2）创建listener
（3）将listener加入epoll slot
**/

class CIncoming
{
public:
	CIncoming();
	void init();
	int processIncoming();
	void addnotify(int fd);

private:
	CPollerUnit pollerUnit;
};

#endif