#include <iostream>
#include <sys/epoll.h>

using namespace std;

#include "pollerobject.h"
#include "../utils/log/log.h"

CPollerObject::CPollerObject() : fd(-1), seq(0), pollunit(NULL)
{}

CPollerObject::~CPollerObject()
{
	log_debug("~CPollerObject()");
}

void CPollerObject::setFd(int _fd)
{
	fd = _fd;
	CSocket::set_nonblock(fd);
}

int CPollerObject::getFd()
{
	return fd;
}

void CPollerObject::setSeq(unsigned int _seq)
{
	seq = _seq;
}

int CPollerObject::getSeq()
{
	return seq;
}

/**
功能：当连接准备就绪，加入监听集
**/
void CPollerObject::onConnReady(CPollerUnit &pollerUnit)
{
	pollunit = &pollerUnit;

	if(attachPoller(pollerUnit) == -1)
	{
		log_debug("attach poller failed.");

		string resp = "attach failed.";
		::write(fd, resp.c_str(), resp.size());
		closeFd();
	}
}

/**
功能：当连接消失时，移除监听集并关闭连接，最后将本身对象释放。
code tips:
在类的成员函数中能不能调用delete this？答案是肯定的.
调用release之后还能调用其他的方法，但是有个前提：被调用的方法不涉及这个对象的数据成员和虚函数。
因为：在类对象的内存空间中，只有数据成员和虚函数表指针，并不包含代码内容，类的成员函数单独放在代码段中。在delete this之后进行的其他任何函数调用，只要不涉及到this指针的内容，都能够正常运行。
**/
void CPollerObject::onConnOver()
{
	closeFd();

	if(pollunit)
	{
		detachPoller(*pollunit);
	}
}

/**
功能：打开EPOLLIN事件。在EPOLLET模式下，EPOLLIN触发之后会被删除，故触发后需要重新注册。
EPOLLET模式下，没有必要反注册EPOLLIN事件。
**/
void CPollerObject::enableInput()
{
	epoll_event event;
	event.events = EPOLLIN | EPOLLET;
	event.data.u32 = seq;
	epoll_ctl(pollunit->epfd, EPOLL_CTL_MOD, fd, &event);
}

/**
功能：打开EPOLLOUT事件，触发返回数据发送到客户端。
EPOLLET模式下，EPOLLOUT触发之后会被删除，在用户数据没有发送完毕之前，需要重新注册EPOLLOUT.
EPOLLET模式下，没有必要反注册EPOLLOUT事件。
**/
void CPollerObject::enableOutput()
{
	epoll_event event;
	event.events = EPOLLIN | EPOLLOUT | EPOLLET;
	event.data.u32 = seq;
	epoll_ctl(pollunit->epfd, EPOLL_CTL_MOD, fd, &event);
}

/**
功能：打开EPOLLOUT事件，触发返回数据发送到客户端。
EPOLLET模式下，没有必要反注册EPOLLOUT事件。
**/
void CPollerObject::disableOutput()
{
	epoll_event event;
	event.events = EPOLLIN | EPOLLET;
	event.data.u32 = seq;
	epoll_ctl(pollunit->epfd, EPOLL_CTL_MOD, fd, &event);
}

/**
功能：将自己加入监听集
返回值：
成功0，失败-1
**/
int CPollerObject::attachPoller(CPollerUnit &pollerUnit)
{
	shared_ptr<CPollerObject> shared_this = shared_from_this();

	int ret = pollerUnit.addPollerObj(shared_this);

	return ret;
}

/**
功能：如果有连接关闭，释放连接对象
返回值：
成功0，失败-1
**/
void CPollerObject::detachPoller(CPollerUnit &pollerUnit)
{
	pollerUnit.delPollerObj(seq);
}

void CPollerObject::closeFd()
{
	::close(fd);
}