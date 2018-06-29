#ifndef _DEFAULT_CTL_H_
#define _DEFAULT_CTL_H_

#include "../comm/serverbase.h"
#include "../comm/nocopyable.h"
#include "procmonitor.h"

class CDdefaultControl : public CServerBase, NoCopyable
{
public:
	CDdefaultControl();
	virtual ~CDdefaultControl();

	void realrun();

private:
	CMonitorProcSrv monsrv;
};

#endif