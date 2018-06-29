#include "../utils/log/log.h"
#include "pollerunit.h"

CPollerUnit::CPollerUnit(int pollnum):maxpollers(pollnum)
{
	events = (epoll_event *)calloc(maxpollers, sizeof(epoll_event));
	init();
}

CPollerUnit::~CPollerUnit()
{
	if(events)
	{
		delete events;
	}
}

void CPollerUnit::init()
{
	createEpoll();
}

/**
创建epoll
（1）LT是默认模式，事件注册并且触发之后不删除，IN事件直到接收缓冲区为空，OUT事件直到发送缓冲区为满。
当IN事件发生后，把数据接收到应用层缓存中，并判断是否有完整包。
当需要发送数据时，注册EPOLLOUT并触发，当应用数据发送完毕后，反注册OUT事件，避免重复不必要的OUT事件触发。
（2）ET模式下当事件到达时，第一会向应用层通知事件，第二回取消事件的注册。
当IN事件发生后，需要把所有接收缓冲区中数据接收，数据接收完毕后，需要继续注册IN事件；
当需要发送数据时，如果应用数据没有发送完全，需要继续注册OUT事件。
**/
void CPollerUnit::createEpoll()
{
	if((epfd = epoll_create(maxpollers)) < 0)
	{
		log_debug("epoll create error. errno=%d", errno);
		exit(0);
	}
}

/**
等待I/O事件到来
**/
EpollResult CPollerUnit::waitPollerEvents(int timeout)
{
	errno = 0;

	int pollnum = epoll_wait(epfd, events, maxpollers, timeout);
	if(pollnum < 0)
	{
		if(errno != EINTR)
		{
			log_error("epoll_wait error. errno=%d", errno);
			exit(0);
		}
		pollnum = 0;
	}

	if(pollnum > 0)
	{
		log_debug("proxy waitPollerEvents...pollnum=%d", pollnum);
	}
	
	return EpollResult(events, pollnum);
}

/**
struct epoll_event {
    unsigned long events;
    union {
        void *ptr;
        int fd;
        unsigned long u32;
        unsigned long long u64;
    } data;
};
处理epoll_event事件
**/
void CPollerUnit::processPollerEvents(EpollResult &result)
{
	//log_debug("proxy processPollerEvents...");

	for(EpollResult::iterator itor = result.begin(); itor != result.end(); itor++)
	{
		epoll_event &event = (epoll_event &)*itor;
		unsigned int seq = event.data.u32;
		log_debug("event.data.u32=%d", seq);
		shared_ptr<CPollerObject> pollerobj = getPollerObj(seq);
		if(pollerobj)
		{
			if(!(event.events & EPOLLOUT) && !(event.events & EPOLLIN))
			{
				pollerobj->onConnOver();
				continue;
			}

			if(event.events & EPOLLOUT)
			{
				if(!pollerobj->outputNotify())
				{
					continue;
				}
			}

			if(event.events & EPOLLIN)
			{
				if(pollerobj != NULL)
				{
				  pollerobj->inputNotify();
				}
			}

		}
	}
	
}

/**
新连接建立后，将新连接（抽象为CPollerObject）加入slot
**/
int CPollerUnit::addPollerObj(shared_ptr<CPollerObject> pollerobj)
{
	if(pollerobj)
	{
		SlotElement<CPollerObject> * slot = pollerSlot.allocNewSlot();
		if(slot)
		{
			epoll_event event;
			event.events = EPOLLIN | EPOLLET;
			event.data.u32 = slot->getSeq();
			log_debug("event.data.u32=%d", event.data.u32);
			epoll_ctl(epfd, EPOLL_CTL_ADD, pollerobj->fd, &event);

			slot->setElement(pollerobj);
			pollerobj->setSeq(slot->getSeq());
			return 0;
		}
	}

	return -1;
}

/**
连接断开后，将连接（抽象为CPollerObject）移除slot
**/
void CPollerUnit::delPollerObj(unsigned int seq)
{
	SlotElement<CPollerObject> * slot = pollerSlot.getSlotElement(seq);
	if(slot)
	{
		shared_ptr<CPollerObject> pollerobj = slot->getElement();

		if(pollerobj)
		{
			epoll_event event;
			event.data.u32 = slot->getSeq();
			epoll_ctl(epfd, EPOLL_CTL_DEL, pollerobj->fd, &event);

			pollerSlot.freeOldSlot(slot);
		}
		else
		{
			log_error("no pollobj find when remove.");
		}
	}
}

/**
当连接有I/O事件来临时，获取连接（抽象为CPollerObject）
**/
shared_ptr<CPollerObject> CPollerUnit::getPollerObj(unsigned int seq)
{
	SlotElement<CPollerObject> * slot = pollerSlot.getSlotElement(seq);
	if(slot)
	{
		return slot->getElement();
	}

	return NULL;
}