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
		// ��ȡfifo����������
		comsumer->readNofiy();

		if(module == "proxy")
		{
			// ����consumer�е���Ϣ�������ظ��ͻ���
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
			// ����consumer�е���Ϣ��������handle_process����
			char *recvbuff = commItem->consumeBuff;
			unsigned int buffsize = commItem->comsumeBuffSize;
			unsigned int datalen;
			int seq;
			int ret = comsumer->consume((void *)recvbuff, buffsize, datalen, seq);
			while(ret == 0)
			{
				//CClientData clientdata;
				//clientdata.write((char *)&seq, sizeof(int));
				//clientdata.write(recvbuff, datalen);
				Benchadapter::benchso.bench_handle_process(recvbuff, datalen, seq, (void *)commItem.get());

				ret = comsumer->consume((void *)recvbuff, buffsize, datalen, seq);
			}
		}
	}
}

bool Notifier::outputNotify()
{
	return true;
}