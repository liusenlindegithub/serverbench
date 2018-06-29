#include "csocket.h"
#include "listener.h"
#include "client.h"

CListener::CListener(int port) 
{
	newfds.clear();
	init(port);
}

CListener::~CListener() { }

void CListener::inputNotify()
{
	newfds.clear();

	int count = handleAccept();
	if(count > 0)
	{
		for(vector<int>::iterator itor = newfds.begin(); itor != newfds.end(); itor++)
		{
			int newfd = *itor;
			shared_ptr<CPollerObject> clientPoller(new CClient(newfd, TCP_SOCKET));
			clientPoller->onConnReady(*pollunit);
		}
	}
}

bool CListener::outputNotify()
{
	return true;
}

/**
ET接收完所有连接
返回值：本次建立新连接数量
**/
int CListener::handleAccept()
{
	int count = 0;

	struct sockaddr_in peer;
	socklen_t peersize = sizeof(struct sockaddr_in);
	while(true)
	{
		int newfd = ::accept(fd, (struct sockaddr *)&peer, &peersize);
		if(newfd <= 0)
		{
			if(errno == EINTR) continue;
			else if(errno == EAGAIN) break;
			else
			{
				log_error("accept error. errno=%d", errno);
				break;
			}
		}
		else
		{
			newfds.push_back(newfd);
			count++;
		}
	}

	return count;
}

/**
设置缓冲区大小：
1. 如果SO_SENDBUF选项值的2倍超过最大值，那么就设置为最大值。编程检测得知服务器最大发送缓冲区值为：425984
2. 如果SO_SNDBUF选项值的2倍小于套接口SO_SNDBUF的最小值，那么实际的SO_SNDBUF则会设置为SO_SNDBUF的最小值。编程检测得知服务器最小发送缓冲区值为：4608
3. 如果SO_SNDBUF选项值的2倍大于套接口SO_SNDBUF的最小值，那么实际的SO_SNDBUF则会设置为SO_SNDBUF的2倍。
同理：
服务器最大接收缓冲区值为：425984
服务器最小接收缓冲区值为：2304
**/
int CListener::init(int port)
{
	int lfd = CSocket::create(TCP_SOCKET);
	if(lfd > 0)
	{
		CSocket::set_reuseaddr(lfd);

		if(CSocket::bind(lfd, port) == 0)
		{
			if(CSocket::listen(lfd) == 0)
			{
				fd = lfd;
				CSocket::set_nonblock(fd);
				
				return 0;
			}
		}
	}

	return -1;
}