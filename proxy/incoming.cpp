#include <memory>
#include <iostream>
#include <list>

#include "../utils/log/log.h"

#include "incoming.h"
#include "../netcomm/cmqcommu.h"
#include "../netcomm/notifier.h"
#include "../netcomm/csocket.h"
#include "../netcomm/listener.h"
#include "../netcomm/udplistener.h"


using namespace std;

CIncoming::CIncoming()
{
	
}

void CIncoming::init()
{
	// ∂¡»°socket≈‰÷√
	list<socket_conf> sock_confs;
	socket_conf sconf_tcp;
	sconf_tcp.socket_type = TCP_SOCKET;
	sconf_tcp.port = 8881;
	socket_conf sconf_udp;
	sconf_udp.socket_type = UDP_SOCKET;
	sconf_udp.port = 8882;
	sock_confs.push_back(sconf_tcp);
	sock_confs.push_back(sconf_udp);

	// ≥ı ºªØsocket
	for(list<socket_conf>::iterator itor=sock_confs.begin(); itor != sock_confs.end(); itor++)
	{
		socket_conf &sock_conf = *itor;
		if(sock_conf.socket_type == TCP_SOCKET)
		{
			shared_ptr<CListener> listener = shared_ptr<CListener>(new CListener(sock_conf.port));
			listener->onConnReady(pollerUnit);
		}
		else if(sock_conf.socket_type == UDP_SOCKET)
		{
			shared_ptr<CUDPListener> listener = shared_ptr<CUDPListener>(new CUDPListener(sock_conf.port));
			listener->attachPoller(pollerUnit);
		} 
		else
		{

		}
	}
}

int CIncoming::processIncoming()
{
	EpollResult result =  pollerUnit.waitPollerEvents(1000);
	pollerUnit.processPollerEvents(result);

	return 0;
}

void CIncoming::addnotify(int fd)
{
	shared_ptr<Notifier> notifier = shared_ptr<Notifier>(new Notifier(fd, "proxy"));
	notifier->onConnReady(pollerUnit);
}
