#include <unistd.h>
#include <iostream>
#include <memory>

using namespace std;

#include "defaultproxy.h"
#include "../utils/log/log.h"
#include "../netcomm/benchadapter.h"
#include "../netcomm/clientdata.h"
#include "../netcomm/pollerobject.h"
#include "../utils/comm/singleton.h"
#include "../netcomm/processcomm.h"

#include "../utils/log/log.h"

CDefaultProxy::CDefaultProxy()
{
	
}

CDefaultProxy::~CDefaultProxy()
{

}

void CDefaultProxy::initconf()
{
	// ׼����־Ŀ¼
	Log::set_log_dir("log/server", "server");

	// ͨѶ׼��(ֻ��һ��worker�������)
	ProcessComm *proCommInstance = ProcessComm::getInstance();

	int groupid = 1;
	shared_ptr<ProcessCommItem> commItem = shared_ptr<ProcessCommItem>(new ProcessCommItem());
	commItem->init("proxy", groupid, 2048);
	int nofiyfd = commItem->getNofiyFd();
	incoming.addnotify(nofiyfd);
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

void CDefaultProxy::realrun()
{
	initconf();
	incoming.init();

	time_t lastmontime = time(NULL) - 5;
	while(true)
	{
		incoming.processIncoming();

		// ��ʱ��controller��������
		time_t now = time(NULL);
		if((now - lastmontime) >= 5)
		{
			procmonCli.sendProcMsg(0, now);
			lastmontime = now;
		}

		// ����Ƿ�quit
		if(isquit())
		{
			break;
		}
	}
}