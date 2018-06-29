#ifndef _PROC_MONITOR_H_
#define _PROC_MONITOR_H_

#include <memory>
#include <list>
#include <vector>
#include <string>
#include "../netcomm/cmqcommu.h"
#include "../comm/cprocmon.h"

using namespace std;

#define MAX_PROC_GROUP_NUM 256

/**
进程信息
**/
class CProc
{
public:
	CProc();
	CProc(int procid, time_t timestamp);

	bool operator==(const CProc &r);

	int procid;
	time_t timestamp; //上一次心跳时间戳
};

/**
进程组信息
**/
class CGroupInfo
{
public:
	CGroupInfo();
	CGroupInfo(int groupid, string groupname, string execfile, unsigned int maxprocnum, unsigned int minprocnum);
	CGroupInfo(const CGroupInfo &right);
	CGroupInfo& operator=(const CGroupInfo &right);

	int    groupid;
	string groupname;
	string execfile;
	unsigned int maxprocnum;
	unsigned int minprocnum;
};

/**
启动的进程组
**/
class CProcGroup
{
	friend class CMonitorProcSrv;
public:
	CProcGroup(const CGroupInfo &groupinfo);

	void addproc(CProc &proc);

private:
	CGroupInfo groupinfo;
	list<CProc> proclist;
};

class ProcEvent
{
	friend class CMonitorProcSrv;
public:
	ProcEvent(int eventid, int groupid, int procid);
private:
	int eventid;
	int groupid;
	int procid;
};

/**
进程监控类
启动时加载预分配的进程信息；
通过接收心跳包,检查进程是否停止工作；
通过检查实际启动的进程信息和预分配的进程信息，启动或杀死相关进程。
**/
class CMonitorProcSrv
{
public:
	CMonitorProcSrv();
	void initconf();
	void run();
	void killallproc();

private:
	vector<CProcGroup> procgroup;
	shared_ptr<CMQCommu> mqcomm;
	ProcMonMsg promsg[2]; // 0 -- 收, 1 -- 发
	list<ProcEvent> events;
	
	void doRecv(int msgtype);
	void doCheck();

	CProc& findProc(int groupid, int procid);
	void   addProc(ProcMsg &promsg);
	void   delProc(int groupid, int procid);
	void   showProc();
	void   doEvent();
};

#endif