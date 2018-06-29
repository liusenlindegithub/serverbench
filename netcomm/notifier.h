#ifndef _NOTIFIER_H_
#define _NOTIFIER_H_

#include <string>

#include "pollerobject.h"

using namespace std;

class Notifier : public CPollerObject
{
public:
	Notifier(int fd, string _module);
	void inputNotify();
	bool outputNotify();

private:
	string module; // 区分proxy和worker在管道消息触发后的不同处理
};

#endif