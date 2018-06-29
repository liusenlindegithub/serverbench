#ifndef _C_PROCMON_H_
#define _C_PROCMON_H_

#include "../netcomm/cmqcommu.h"
#include <memory>

using namespace std;

#define HEART_BEAT_MQID 255

#define PROCMON_EVENT_PROCDOWN (1<<1)
#define PROCMON_EVENT_PROCUP   (1<<2)
#define PROCMON_EVENT_PROCDEAD (1<<3)

typedef enum
{
	PROC_INFO_TICK = 0,
	PROC_EVENT
}PROC_MSG_TYPE;

typedef struct
{
	int groupid;
	int procid;
	time_t timestamp;
}ProcMsg;

/**
由各进程组发送定时消息给controller的消息结构
**/
typedef struct 
{
	long msgtype;  // 转化成struct msgbuf所必须
	PROC_MSG_TYPE  type;     // 消息分类
	union
	{
		ProcMsg tickmsg;
	}body;
}ProcMonMsg;


/**
心跳客户端, 由各进程组发送定时消息给controller
**/
class CProcMonitorCli
{
public:
	CProcMonitorCli();
	void sendProcMsg(int groupid, int timestamp);
private:
	ProcMonMsg procmonmsg;
	shared_ptr<CMQCommu> mqcomm;
	void contructProcMsg(int groupid, int timestamp);
};

#endif