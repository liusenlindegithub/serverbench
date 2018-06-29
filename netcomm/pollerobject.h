#ifndef _POLLER_OBJECT_H_
#define _POLLER_OBJECT_H_

#include <memory>
using namespace std;

#include "csocket.h"
#include "pollerunit.h"


class CPollerUnit;
class CPollerObject : public enable_shared_from_this<CPollerObject>
{
public:
	friend class CPollerUnit;

	CPollerObject();
	virtual ~CPollerObject();

	int getFd();
	void setFd(int _fd);
	int getSeq();
	void setSeq(unsigned int _seq);

	void onConnReady(CPollerUnit &pollerUnit);
	void onConnOver();

	void enableInput();
	void enableOutput();
	void disableOutput();
	virtual void inputNotify() = 0;
	virtual bool outputNotify() = 0;

protected:
	int attachPoller(CPollerUnit &pollerUnit);
	void detachPoller(CPollerUnit &pollerUnit);
	void closeFd();

protected:
	int fd;
	unsigned int seq;
	CPollerUnit *pollunit;
	
};

#endif