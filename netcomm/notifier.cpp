#include "notifier.h"
#include "processcomm.h"
#include "client.h"
#include "benchadapter.h"
#include "clientdata.h"

Notifier::Notifier(int fd, string _module)
{
	setFd(fd);
	module = _module;
}

void Notifier::inputNotify()
{
	ProcessComm *proCommInstance = ProcessComm::getInstance();
	shared_ptr<ProcessCommItem> commItem = proCommInstance->getNtor(fd);
	shared_ptr<ShmConsumer> comsumer(NULL);
	if(commItem)
	{
		comsumer = commItem->getConsumer();
	}

	if(comsumer)
	{
		// 读取fifo中所有数据
		comsumer->readNofiy();

		if(module == "proxy")
		{
			// 接收consumer中的消息，并返回给客户端
			char *recvbuff = commItem->consumeBuff;
			unsigned int buffsize = commItem->comsumeBuffSize;
			unsigned int datalen;
			int seq;
			int ret = comsumer->consume((void *)recvbuff, buffsize, datalen, seq);
			while(ret == 0)
			{
				shared_ptr<CPollerObject> pollobject = pollunit->getPollerObj(seq);
				if(pollobject != NULL)
				{
					CClient *client = dynamic_cast<CClient *>(pollobject.get());
					client->sentResponseCache(recvbuff, datalen);
					client->enableOutput();
				}
				
				ret = comsumer->consume((void *)recvbuff, buffsize, datalen, seq);
			}
		}
		else if(module == "worker")
		{
			// 接收consumer中的消息，并调用handle_process处理
			char *recvbuff = commItem->consumeBuff;
			unsigned int buffsize = commItem->comsumeBuffSize;
			unsigned int datalen;
			int seq;
			int ret = comsumer->consume((void *)recvbuff, buffsize, datalen, seq);
			while(ret == 0)
			{
				CClientData clientdata;
				clientdata.write((char *)&seq, sizeof(int));
				clientdata.write(recvbuff, datalen);
				Benchadapter::benchso.bench_handle_process((void *)&clientdata, (void *)commItem.get());

				ret = comsumer->consume((void *)recvbuff, buffsize, datalen, seq);
			}
		}
	}
}

bool Notifier::outputNotify()
{
	return true;
}