#include <signal.h>

#include <iostream>
 
using namespace std;

#include "serverbase.h"

int CServerBase::sigflag = 0;

CServerBase::CServerBase()
{
	
}

CServerBase::~CServerBase()
{

}

void CServerBase::run()
{
	startup();
	realrun();
}


void CServerBase::startup()
{
	signal(SIGUSR1, CServerBase::sigusr1_handle);
	signal(SIGUSR2, CServerBase::sigusr2_handle);
}

void CServerBase::realrun()
{
	
}

void CServerBase::sigusr1_handle(int signo)
{
	sigflag |= SIG_FLAG_QUIT;
}

void CServerBase::sigusr2_handle(int signo)
{
	
}

bool CServerBase::isquit()
{
	if(sigflag & SIG_FLAG_QUIT)
	{
		sigflag &= ~SIG_FLAG_QUIT;
		return true;
	}
	else
	{
		return false;
	}
}
