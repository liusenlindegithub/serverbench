#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "pollerobject.h"
#include "clientdata.h"

typedef enum
{
	E_RECV_COMPLETE = 0,
	E_RECV_CLOSE,
	E_RECV_ERROR
}RECV_DATA_CODE;

class CClient : public CPollerObject
{
public:
	CClient(int newfd, int clntype);
	void inputNotify();
	bool outputNotify();
	void sentResponseCache(char *data, int len);

private:
	int clientType;
	CClientData wDataCache;
	CClientData rDataCache;

	RECV_DATA_CODE recvData();
	void processData();
	int sendData();
};

#endif