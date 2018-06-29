#ifndef _C_MSGQUEUE_COMMU_H_
#define _C_MSGQUEUE_COMMU_H_

/**
System V 消息队列，用于进程间消息同步；
相关原理：
在内核中保存消息链表，由用户进程指明消息类型和内容，这些信息被放置到预定于的消息结构中，插入到消息链表中。
用户进程可以根据消息类型，有选择地从队列中按照FIFO原则读取特定类型消息。
消息队列数据结构：
struct msqid_ds {
	struct ipc_perm msg_perm; // 进程对消息队列的权限
	struct msg_message *msg_first; //第一个消息
	struct msg_message *msg_last;  //最后一个消息
	...
};

struct msg_message {
	struct msg *msg; //进程消息体
	int m_ts; //进程消息大小
	long m_type; //进程消息类型
	struct msg_message *next;
	...
};

struct msgbuf {
   long mtype;       // message type, must be > 0
   char mtext[1];    // message data 
};
打开或创建消息队列:
int msgget(key_t key, int msgflag);
* key: 一般由 ftok(const char * fname, int id) 生成。
* msgflag： IPC_CREAT:创建新的消息队列。 IPC_EXCL:与IPC_CREAT一同使用，表示如果要创建的消息队列已经存在，则返回错误。 IPC_NOWAIT:读写消息队列要求无法满足时，不阻塞。

消息发送：
int msgsnd(int msqid, struct msgbuf *msgp, size_t msgsz, int msgflg); 
* msgsz ：的大小指定为 mtext 的大小；
* msgflg ：指定为IPC_NOWAIT时，当空间满时返回-1,errno=EAGAIN

消息接收：
ssize_t msgrcv(int msqid, struct msgbuf *msgp, size_t msgsz, long msgtyp, int msgflg);
* msgsz ：设置为 mtext的大小；
* msgtyp ： 设置为0表示接收队列中第一个消息；>0表示接收队列中第一个msgtyp类型的消息；<0表示接收队列中msgtyp类型最小的那个消息；
* msgflg ：指定为IPC_NOWAIT时，当空间空时返回-1,errno=ENOMSG.
**/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

class CMQCommu
{
public:
	CMQCommu(int id);
	virtual ~CMQCommu();

	static key_t pwdtokey(int id);
	int send(struct msgbuf *msg, int msgsize);
	int recv(struct msgbuf *msg, int msgsize, long msgtype = 0);
private:
	int init(int id);
	void fini();

	int mqid;
};

#endif