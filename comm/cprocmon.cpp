#include <sys/types.h>
#include <unistd.h>
#include <iostream>

#include "cprocmon.h"

using namespace std;

CProcMonitorCli::CProcMonitorCli()
{
	mqcomm = shared_ptr<CMQCommu>(new CMQCommu(HEART_BEAT_MQID));
}

void CProcMonitorCli::sendProcMsg(int groupid, int timestamp)
{
	contructProcMsg(groupid, timestamp);
	mqcomm->send((struct msgbuf *)&procmonmsg, sizeof(ProcMonMsg) - sizeof(long));
}

void CProcMonitorCli::contructProcMsg(int groupid, int timestamp)
{
	procmonmsg.msgtype = getpid();
	procmonmsg.type = PROC_INFO_TICK;

	ProcMsg &procmsg = procmonmsg.body.tickmsg;
	procmsg.groupid = groupid;
	procmsg.procid = getpid();
	procmsg.timestamp = timestamp;
}