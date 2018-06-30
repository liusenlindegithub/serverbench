#include <iostream>

#include "client.h"
#include "csocket.h"
#include "cbregister.h"
#include "../utils/log/log.h"
#include "../utils/comm/singleton.h"
#include "benchadapter.h"
#include "processcomm.h"

using namespace std;

CClient::CClient(int newfd, int clntype)
{
	clientType = clntype;
	fd = newfd;
	CSocket::set_nonblock(fd);
}

/**
功能：当接收到请求时，ET接收完成所有数据 -> 完成用户端收包 -> 发包处理
潜在问题 : 当远程客户端在发送数据的途中关闭了连接，则服务端接收缓存里就存在了 "脏数据"，影响后面的数据处理。
**/
void CClient::inputNotify()
{
	RECV_DATA_CODE ret = recvData();

	if(ret == E_RECV_COMPLETE)
	{
		enableInput();
		processData();
	}
	else if(ret == E_RECV_CLOSE)
	{
		log_debug("----- request for close...");
		onConnOver();
	}
	else
	{
		onConnOver();
	}
}

/**
功能：向客户端发送返回，暂时不支持UDP返回
**/
bool CClient::outputNotify()
{
	bool nextevent = true;
	int size = sendData();

	if(size >= 0)
	{
		if(rDataCache.mReadPos < rDataCache.mWritePos) // 数据没有发完，继续注册EPOLLOUT
		{
			enableOutput();
		}
		else
		{
			rDataCache.alignPosition();
			//disableOutput();  // 这里可以不调用
		}
	}
	else
	{
		onConnOver();
		nextevent = false;
	}
	
	return nextevent;
}

/**
功能：业务层调用，发送返回数据
**/
void CClient::sentResponseCache(char *data, int len)
{
	rDataCache.write(data, len);
}

/**
功能：非阻塞模式下数据接收
（1）当
返回值：
**/
RECV_DATA_CODE CClient::recvData()
{
	RECV_DATA_CODE retCode;

	char buff[1024];
	bool isover = false;
	//errno = 0;
	while(!isover)
	{
		int size = read(fd, buff, sizeof(buff));
		//log_debug("---recv:%s, size=%d, errno=%d", buff, size, errno);
		if(size > 0)
		{
			wDataCache.write(buff, size);
		}
		else if(size == 0)
		{
			retCode = E_RECV_CLOSE;
			isover = true;
		}
		else
		{
			if(errno == EAGAIN)
			{
				retCode = E_RECV_COMPLETE;
				isover = true;
			}
			else if(errno == EINTR)
			{
				continue;
			}
			else
			{
				log_error("recv data error. errno=%d", errno);
				retCode = E_RECV_ERROR;
				isover = true;
			}
		}
	}

	return retCode;
}

void CClient::processData()
{
	int seq = getSeq();
	CClientData *dataCache = &wDataCache;
	CPollerObject *pollobj = this;

	int protoLen = 0;
	do
	{
		protoLen = Benchadapter::benchso.bench_handle_input(dataCache, NULL);
		if(protoLen > 0)
		{
			int readbleSize = 0;
			char *cachedata = dataCache->getReadbleData(readbleSize);
			
			int routeid = Benchadapter::benchso.bench_handle_route(dataCache, pollobj); // 以groupid代表routeid，表示路由到哪个worker		
			ProcessComm *processcomm = ProcessComm::getInstance();
			shared_ptr<ProcessCommItem> commItem = processcomm->getRtor(routeid);
			if(commItem)
			{
				shared_ptr<ShmProducer> producer = commItem->getProducer();
				if(producer)
				{
					producer->produce(cachedata, protoLen, seq);
					producer->sendNotify();
				}
				else
				{
					log_error("NULL Point error. no share memory pool of producer!");
					cout << "NULL Point error. no share memory pool of producer.!" << endl;
				}
			}
			else
			{
				log_error("NULL Point error. cannot find process communication item by route id!");
				cout << "NULL Point error. cannot find process communication item by route id!" << endl;
			}

			dataCache->alignPosition(protoLen);
		}
	}
	while (protoLen > 0);
}

/**
功能：非阻塞模式下发送数据
（1）当对端断开时，如果注册了IN和OUT事件，它们都会被触发。
（2）当发送缓冲区满时，write返回-1，非阻塞情况下errno=EAGAIN
（3）模拟情况：server不停向client发送数据，client始终不read，导致client接收缓冲区满，server发送缓冲区变满。这时server不断发送tcp试探包，客户端返回的窗口大小始终为0。最后，client向server发送RST报文，服务端内核断开连接，并向应用层通知OUT和IN事件。server先在OUT事件触发时，根据write返回-1（errno=EAGAIN），回收所有在该连接上分配的内存。
返回值：已发送字节数
**/
int CClient::sendData()
{
	int totalsize = rDataCache.mWritePos - rDataCache.mReadPos;

	if(clientType == TCP_SOCKET)
	{
		int size = ::write(fd, rDataCache.mData + rDataCache.mReadPos, totalsize);
		log_debug("send resp size=%d, errno=%d", size,errno);
		if(size >= 0)
		{
			rDataCache.mReadPos += size;
			return size;
		}
		else
		{
			log_error("send to client error. errno=%d", errno);
			return -1;
		}
	}
	else if(clientType == UDP_SOCKET)
	{
	    // 暂时不支持UDP返回
		/*
		struct sockaddr_in clientAddr;
		//int size = ::sendto(fd, rDataCache.mData + rDataCache.mReadPos, totalsize, 0, (struct sockaddr *)&clientAddr, sizeof(clientAddr));
		char buff[2048]={0};
		int len=5;
		const char *msg = "hello";
		memcpy(buff, &len, sizeof(len));
		memcpy(buff + sizeof(len), msg, len);
		int size = ::sendto(fd, buff, len+sizeof(len), 0, (struct sockaddr *)&clientAddr, sizeof(clientAddr));
		if(size >= 0)
		{
			log_error("send to client hello. errno=%d", errno);
			rDataCache.mReadPos += size;
			return size;
		}
		else
		{
			log_error("send to client error. errno=%d", errno);
			return -1;
		}
		*/
	}
	
}
