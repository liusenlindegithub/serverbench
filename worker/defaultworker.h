#ifndef _DEFAULT_WORKER_H_
#define _DEFAULT_WORKER_H_

#include "../comm/serverbase.h"
#include "../netcomm/pollerunit.h"
#include "../netcomm/notifier.h"

class CDefaultWorker : public CServerBase
{
public:
	void realrun();
	void initconf();

private:
	CPollerUnit pollerunit;
};

#endif