#ifndef _UDP_LISTENER_H_
#define _UDP_LISTENER_H_

using namespace std;

#include "pollerunit.h"
#include "client.h"

/**
UDP¼àÌý
**/

class CUDPListener
{
public:
	CUDPListener(int port);
	~CUDPListener();
	int attachPoller(CPollerUnit &pollerUnit);

private:
	int lfd;
	shared_ptr<CClient> listener;

	int init(int port);
};

#endif

