#ifndef _SPP_PROXY_DEFAULT_H_
#define _SPP_PROXY_DEFAULT_H_

#include "../comm/serverbase.h"
#include "../comm/nocopyable.h"
#include "incoming.h"

class CDefaultProxy : public CServerBase, NoCopyable
{
public:
	CDefaultProxy();
	virtual ~CDefaultProxy();

	void initconf();
	void realrun();

private:
	CIncoming incoming;
};

#endif