/**
用于proxy和worker之间的通信。
proxy负责解析数据包，路由数据包给worker；
worker负责处理数据包业务逻辑；
一个proxy对应多个worker，且每个proxy和worker的通信使用一组共享内存，每一组共享内存实际上有produer和consumer两个共享内存块，用于proxy与worker之间相互发送数据的缓存。
以上逻辑全部封装在ProcessComm类中，该对象由proxy或worker层创建，并以单例形式，由network层调用。
例如：proxy发送消息给worker时，在network层直接调用 ProcessComm::getInstance()->sendto();
**/

#ifndef _PROCESS_COMM_H_
#define _PROCESS_COMM_H_

#include <map>
#include <memory>
using namespace std;

#include "cshmcommu.h"
#include "fifocomm.h"

class ShmProducer
{
public:
	ShmProducer(int shmkey, int shmsize);
	void setNotifyer(int key);
	int produce(const void *data, unsigned int datalen, int seq);
	void sendNotify();
private:
	shared_ptr<CShmMQ> mq;
	shared_ptr<FifoComm> fifo;
	int notify_fd;
};

class ShmConsumer
{
public:
	ShmConsumer(int shmkey, int shmsize);
	void setNotifyer(int key);
	int consume(void *buff, unsigned int buffsize, unsigned int &datalen, int &seq);
	void readNofiy();
	int getNofiyFd();
private:
	shared_ptr<CShmMQ> mq;
	shared_ptr<FifoComm> fifo;
	int notify_fd;
};

class ProcessCommItem
{
public:
	ProcessCommItem();
	virtual ~ProcessCommItem();
	void init(string creator, int groupid, unsigned int shmsize);
	int getNofiyFd();  // 消费前的唤醒fifo
	shared_ptr<ShmProducer> getProducer();
	shared_ptr<ShmConsumer> getConsumer();
private:
	key_t pwdtokey(int id);

public:
	char *consumeBuff;  //从consumer获取消息临时存放地
	unsigned int comsumeBuffSize;
private:
	shared_ptr<ShmProducer> producer;
	shared_ptr<ShmConsumer> consumer;
	int monitFd; // 消费前的唤醒fifo
};

class ProcessComm
{
public:
	static ProcessComm *instance;
	static ProcessComm *getInstance();
	
	void addRtor(int groupid, shared_ptr<ProcessCommItem> &r);
	void addNtor(int notifyfd, shared_ptr<ProcessCommItem> &r);
	shared_ptr<ProcessCommItem> getRtor(int groupid);
	shared_ptr<ProcessCommItem> getNtor(int notifyid);

private:
	ProcessComm();

private:
	map<int, shared_ptr<ProcessCommItem> > ctor; // 保存 groupid - ProcessCommItem 的对应关系,在proxy按路由分发消息时使用；
	map<int, shared_ptr<ProcessCommItem> > ator; // 保存 monitFd - ProcessCommItem 的对应关系,当monitFd触发有消息可以消费时，查找消费者在哪
};

#endif