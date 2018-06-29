#include <unistd.h>
#include <iostream>
#include <cstring>
#include <libgen.h>

#include "../utils/log/log.h"
#include "../utils/comm/crc32.h"
#include "cmqcommu.h"

using namespace std;
using namespace platform::commlib;

CMQCommu::CMQCommu(int id):mqid(0)
{
	init(id);
}

CMQCommu::~CMQCommu()
{
	fini();
}

int CMQCommu::init(int id)
{
	key_t key = pwdtokey(id);

	mqid = msgget(key, IPC_CREAT | 0666);
	if(mqid < 0)
	{
		log_error("message queue create failed, mqkey=%d, errno=%d", key, errno);
		exit(-1);
	}

	return 0;
}

/**
这里不可以释放进程间共用的资源。
否则， 一个进程的退出对该资源的释放，对其它进程造成影响。
**/
void CMQCommu::fini()
{
    if (mqid > 0)
    {
        //msgctl(mqid, IPC_RMID, NULL);
    }
}

/**
结合当前程序执行目录和id值，计算出crc32 code，作为进程间通信的消息队列key。
保证一组进程启动后共用当前唯一的key_t资源。
**/
key_t CMQCommu::pwdtokey(int id)
{
	char seed[256];
	readlink("/proc/self/exe", seed, 256);
	cout << "----------------pwdtokey seed1=" << seed << endl;
	dirname(seed);
	cout << "----------------pwdtokey seed2=" << seed << endl;

	int seed_len = strlen(seed) + sizeof(id);
	memcpy(seed + strlen(seed), &id, sizeof(id));

	CCrc32 generator;
	return generator.Crc32((unsigned char *)seed, seed_len);
}

/**
发送消息
参数：
msg：消息体
msgsize：消息大小，已经减去 sizeof(long) 后的值。
**/
int CMQCommu::send(struct msgbuf *msg, int msgsize)
{
	errno = 0;
	int ret = 0;

	do
	{
		ret = msgsnd(mqid, msg, msgsize, IPC_NOWAIT);
	}
	while (ret < 0 && errno == EINTR);
	
	return ret;
}

/**
接收消息
参数：
msg：消息体
msgsize：消息大小，已经减去 sizeof(long) 后的值。
msgtype：队列中消息类型
**/
int CMQCommu::recv(struct msgbuf *msg, int msgsize, long msgtype)
{
	errno = 0;
	int ret = 0;

	do
	{
		ret = msgrcv(mqid, msg, msgsize, msgtype, IPC_NOWAIT);
	}
	while (ret < 0 && errno == EINTR);

	return ret;
}
