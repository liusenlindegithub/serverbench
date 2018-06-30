#include <unistd.h>
#include <iostream>

#include "defaultctl.h"
#include "../utils/log/log.h"

using namespace std;

CDdefaultControl::CDdefaultControl()
{

}

CDdefaultControl::~CDdefaultControl()
{

}

void CDdefaultControl::realrun()
{
	// ׼����־Ŀ¼
	Log::set_log_dir("log/control", "control");
	
	monsrv.initconf();
	
	while(true)
	{
		monsrv.run();
		sleep(5);

		if(CServerBase::isquit())
		{
			monsrv.killallproc();
			break;
		}
	}
}