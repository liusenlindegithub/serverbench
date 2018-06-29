#ifndef _LISTENER_H_
#define _LISTENER_H_

#include <memory>
#include <vector>
using namespace std;

#include "pollerobject.h"

/**
TCP¼àÌý
**/

class CListener : public CPollerObject
{
public:
	CListener(int port);
	~CListener();

	void inputNotify();
	bool outputNotify();

private:
	vector<int> newfds;

	int init(int port);
	int handleAccept();
};

#endif