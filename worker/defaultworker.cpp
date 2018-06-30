#include <unistd.h>
#include <iostream>
#include <memory>

#include "defaultworker.h"
#include "../netcomm/processcomm.h"
#include "../netcomm/benchadapter.h"

#include "../utils/log/log.h"

using namespace std;

void CDefaultWorker::realrun()
{
	initconf();

	time_t lastmontime = time(NULL) - 5;
	while(true)
	{
		EpollResult result =  pollerunit.waitPollerEvents(1000);
		pollerunit.processPollerEvents(result);
		
		// ��ʱ��controller��������
		time_t now = time(NULL);
		if((now - lastmontime) >= 5)
		{
			procmonCli.sendProcMsg(1, now);
			lastmontime = now;
		}

		// ����Ƿ�quit
		if(isquit())
		{
			break;
		}
	}
}

void CDefaultWorker::initconf()
{
	// ׼����־Ŀ¼
	Log::set_log_dir("log/worker", "worker");

	// ͨѶ׼��(ֻ��һ��worker�������)
	ProcessComm *proCommInstance = ProcessComm::getInstance();

	int groupid = 1;
	shared_ptr<ProcessCommItem> commItem = shared_ptr<ProcessCommItem>(new ProcessCommItem());
	commItem->init("worker", groupid, 2048);
	int nofiyfd = commItem->getNofiyFd();
	
	shared_ptr<Notifier> notifier = shared_ptr<Notifier>(new Notifier(nofiyfd, "worker"));
	notifier->onConnReady(pollerunit);

	proCommInstance->addRtor(groupid, commItem);
	proCommInstance->addNtor(nofiyfd, commItem);

	// ���ز��
	if(0 == Benchadapter::benchAdaperLoad("libexample.so"))
	{
		int handle_init_ret;
		handle_init_ret = Benchadapter::benchso.bench_handle_init((void *)"conf_file", this);
		if(handle_init_ret != 0)
		{
			log_debug("bench_handle_init() exec error!");
			exit(-1);
		}
	}	
}
