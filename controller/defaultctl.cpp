#include <unistd.h>
#include <iostream>

#include "defaultctl.h"

using namespace std;

CDdefaultControl::CDdefaultControl()
{

}

CDdefaultControl::~CDdefaultControl()
{

}

void CDdefaultControl::realrun()
{
	cout << "control run..." << endl;
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