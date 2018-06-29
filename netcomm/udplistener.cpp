#include "udplistener.h"

CUDPListener::CUDPListener(int port)
{
	if(init(port) == 0)
	{
		listener = shared_ptr<CClient>(new CClient(lfd, UDP_SOCKET));
	}
	else
	{
		cout << "cannot create upd socket!" << endl;
	}
}

CUDPListener::~CUDPListener()
{
	
}

int CUDPListener::attachPoller(CPollerUnit &pollerUnit)
{
	if(listener)
	{
		listener->onConnReady(pollerUnit);
	}
}



int CUDPListener::init(int port)
{
	int lfd = CSocket::create(UDP_SOCKET);
	if(lfd > 0)
	{
		if(CSocket::bind(lfd, port) == 0)
		{
			this->lfd = lfd;
			return 0;
		}
	}

	return -1;
}

