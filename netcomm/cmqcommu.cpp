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
���ﲻ�����ͷŽ��̼乲�õ���Դ��
���� һ�����̵��˳��Ը���Դ���ͷţ��������������Ӱ�졣
**/
void CMQCommu::fini()
{
    if (mqid > 0)
    {
        //msgctl(mqid, IPC_RMID, NULL);
    }
}

/**
��ϵ�ǰ����ִ��Ŀ¼��idֵ�������crc32 code����Ϊ���̼�ͨ�ŵ���Ϣ����key��
��֤һ������������õ�ǰΨһ��key_t��Դ��
**/
key_t CMQCommu::pwdtokey(int id)
{
	char seed[256];
	readlink("/proc/self/exe", seed, 256);
	dirname(seed);

	int seed_len = strlen(seed) + sizeof(id);
	memcpy(seed + strlen(seed), &id, sizeof(id));

	CCrc32 generator;
	return generator.Crc32((unsigned char *)seed, seed_len);
}

/**
������Ϣ
������
msg����Ϣ��
msgsize����Ϣ��С���Ѿ���ȥ sizeof(long) ���ֵ��
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
������Ϣ
������
msg����Ϣ��
msgsize����Ϣ��С���Ѿ���ȥ sizeof(long) ���ֵ��
msgtype����������Ϣ����
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
