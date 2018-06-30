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
���ܣ������յ�����ʱ��ET��������������� -> ����û����հ� -> ��������
Ǳ������ : ��Զ�̿ͻ����ڷ������ݵ�;�йر������ӣ������˽��ջ�����ʹ����� "������"��Ӱ���������ݴ���
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
���ܣ���ͻ��˷��ͷ��أ���ʱ��֧��UDP����
**/
bool CClient::outputNotify()
{
	bool nextevent = true;
	int size = sendData();

	if(size >= 0)
	{
		if(rDataCache.mReadPos < rDataCache.mWritePos) // ����û�з��꣬����ע��EPOLLOUT
		{
			enableOutput();
		}
		else
		{
			rDataCache.alignPosition();
			//disableOutput();  // ������Բ�����
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
���ܣ�ҵ�����ã����ͷ�������
**/
void CClient::sentResponseCache(char *data, int len)
{
	rDataCache.write(data, len);
}

/**
���ܣ�������ģʽ�����ݽ���
��1����
����ֵ��
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
			
			int routeid = Benchadapter::benchso.bench_handle_route(dataCache, pollobj); // ��groupid����routeid����ʾ·�ɵ��ĸ�worker		
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
���ܣ�������ģʽ�·�������
��1�����Զ˶Ͽ�ʱ�����ע����IN��OUT�¼������Ƕ��ᱻ������
��2�������ͻ�������ʱ��write����-1�������������errno=EAGAIN
��3��ģ�������server��ͣ��client�������ݣ�clientʼ�ղ�read������client���ջ���������server���ͻ�������������ʱserver���Ϸ���tcp��̽�����ͻ��˷��صĴ��ڴ�Сʼ��Ϊ0�����client��server����RST���ģ�������ں˶Ͽ����ӣ�����Ӧ�ò�֪ͨOUT��IN�¼���server����OUT�¼�����ʱ������write����-1��errno=EAGAIN�������������ڸ������Ϸ�����ڴ档
����ֵ���ѷ����ֽ���
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
	    // ��ʱ��֧��UDP����
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
