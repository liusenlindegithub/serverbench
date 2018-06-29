#ifndef _CSHM_COMMU_H_
#define _CSHM_COMMU_H_

#include <sys/ipc.h>
#include <sys/shm.h>
#include <atomic>
using namespace std;

class ShmState
{
public:
	atomic<unsigned int> currMsgNum; //当前消息数量
	atomic<unsigned int> historyMsgNum; //已处理消息数量

	// 由于atomic模版不支持拷贝构造函数，这里如果不显式定义拷贝构造函数，将在隐式拷贝构造函数中调用atomic的拷贝构造，导致编译报错
	ShmState(const ShmState &r):currMsgNum(0),historyMsgNum(0)
	{
	}

	// 由于atomic模版不支持拷贝构造函数，这里如果不显式定义赋值函数，将在隐式赋值函数中调用atomic的拷贝构造，导致编译报错
	ShmState& operator=(const ShmState& r)
	{
		currMsgNum = r.currMsgNum.load();
		historyMsgNum = r.currMsgNum.load();
	}
};

typedef enum
{
	CSHM_ENQUEUE_SUCCESS = 0,
	CSHM_ENQUEUE_NOSPACE,
	CSHM_ENQUEUE_SPACEERR,
	
	CSHM_DEQUEUE_SUCCESS,
	CSHM_DEQUEUE_EMPTY,
	CSHM_DEQUEUE_SPACEERR,
	CSHM_DEQUEUE_BADSPPX_1,
	CSHM_DEQUEUE_BADSPPX_2
}CSHM_Q_RESULT;

/**
使用共享内存构建循环消息队列。
共享内存结构：[状态信息|head|tail|循环消息空间]
状态信息结构：[当前消息数量|已处理消息数量]
消息结构：[SPPX|seq|total_len|body|SPPX] , 其中SPPX为魔法数，在消息出队列时，检查SPPX来判定消息的完整性。
**/
class CShmMQ
{
public:
	CShmMQ();
	virtual ~CShmMQ();
	int createShm(int shmkey, int shmsize);
	void destroyShm();
	CSHM_Q_RESULT enqueue(const void *data, unsigned int datalen, int seq);
	CSHM_Q_RESULT dequeue(void *buff, unsigned int buffsize, unsigned int &datalen, int &seq);

private:
	int shmkey;
	int shmsize; 
	int shmid;
	void *shmaddr; //共享内存起始地址

	ShmState *pstat;
	atomic<unsigned int *> head;//写指针
	atomic<unsigned int *> tail;//读指针
	char *block;   // 消息存放空间起始地址
	unsigned int blocksize; // 消息空间大小

	void printBlock();
};

#endif