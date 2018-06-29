#ifndef _POLLER_UNIT_H_
#define _POLLER_UNIT_H_

#include <sys/epoll.h>
#include <memory>

using namespace std;

#include "../utils/log/log.h"
#include "pollerobject.h"
#include "../comm/slot.h"

#define MAX_POLLER_NUM 1024

class CPollerObject;

class EpollResult
{
public:
	class iterator;

	EpollResult(epoll_event *evnts, int num):events(evnts), pollernum(num) {}
	iterator begin()
	{
		return iterator(0, *this);
	}
	iterator end()
	{
		return iterator(pollernum, *this);
	}

	class iterator
	{
	public:
		iterator(int idx, EpollResult &res):_index(idx), _res(res) {}
		bool operator!=(const iterator &right) 
		{
			return !((_index == right._index) && (&_res == &right._res));
		}
		// 为了区分++和--的前缀和后缀调用，C++规定后缀形式的重载要有一个int参数，当函数调用时，编译器传递一个0作为实参。
		iterator & operator++(int)
		{
			_index++;
			return *this;
		}
		epoll_event & operator*()
		{
			return _res.events[_index];
		}
		epoll_event * operator->()
		{
			return &_res.events[_index];
		}
		epoll_event & at()
		{
			return _res.events[_index];
		}

	private:
		int _index;
		EpollResult &_res;
	};
private:
	epoll_event *events;
	int pollernum;
};

/**
类功能：
管理所有tcp连接，并监听读写事件的到来。
**/
class CPollerUnit
{
public:
	friend class CPollerObject;

	CPollerUnit(int pollnum = MAX_POLLER_NUM);
	virtual ~CPollerUnit();

	EpollResult waitPollerEvents(int timeout);
	void processPollerEvents(EpollResult &pollresult);

	int addPollerObj(shared_ptr<CPollerObject> pollerobj);
	void delPollerObj(unsigned int seq);
	shared_ptr<CPollerObject> getPollerObj(unsigned int seq);

private:
	int epfd;
	int maxpollers;
	epoll_event *events;
	CSlot<CPollerObject> pollerSlot;

	void init();
	void createEpoll();
	
};

#endif