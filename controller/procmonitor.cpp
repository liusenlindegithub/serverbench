#include <unistd.h>
#include <signal.h>
#include <iostream>
#include <algorithm>

#include "../utils/log/log.h"
#include "procmonitor.h"

using namespace std;

CProc::CProc()
{
	procid = -1;
	timestamp = 0;
}

CProc::CProc(int procid, time_t timestamp)
{
	this->procid = procid;
	this->timestamp = timestamp;
}

bool CProc::operator==(const CProc &r)
{
	return (this->procid == r.procid);
}

CGroupInfo::CGroupInfo()
{
	this->groupname = "";
	this->execfile = "";
	this->maxprocnum = 0;
	this->minprocnum = 0;
}

CGroupInfo::CGroupInfo(int groupid, string groupname, string execfile, unsigned int maxprocnum, unsigned int minprocnum)
{
	this->groupid    = groupid;
	this->groupname  = groupname;
	this->execfile   = execfile;
	this->maxprocnum = maxprocnum;
	this->minprocnum = minprocnum;
}

CGroupInfo::CGroupInfo(const CGroupInfo &right)
{
	this->groupid    = right.groupid;
	this->groupname  = right.groupname;
	this->execfile   = right.execfile;
	this->maxprocnum = right.maxprocnum;
	this->minprocnum = right.minprocnum;
}

CGroupInfo& CGroupInfo::operator=(const CGroupInfo &right)
{
	if(this != &right)
	{
		this->groupid    = right.groupid;
		this->groupname  = right.groupname;
		this->execfile   = right.execfile;
		this->maxprocnum = right.maxprocnum;
		this->minprocnum = right.minprocnum;
	}

	return *this;
}

CProcGroup::CProcGroup(const CGroupInfo &groupinfo)
{
	this->groupinfo = groupinfo;
	proclist.clear();
}

ProcEvent::ProcEvent(int eventid, int groupid, int procid)
{
	this->eventid = eventid;
	this->groupid = groupid;
	this->procid  = procid;
}

CMonitorProcSrv::CMonitorProcSrv()
{
	
}

void CMonitorProcSrv::initconf()
{
	// 初始化启动的进程组的基本信息
	procgroup.clear();

	CGroupInfo proxygroupinfo(0, "proxy", "server", 1, 1);
	CProcGroup proxygroup(proxygroupinfo);

	CGroupInfo workergroupinfo(1, "worker", "worker", 4, 1);
	CProcGroup workergroup(workergroupinfo);

	procgroup.push_back(proxygroup);
	procgroup.push_back(workergroup);

	// 删除之前以前残留的消息队列
	key_t mqkey = CMQCommu::pwdtokey(HEART_BEAT_MQID);
	char rmcmd[256] = {0};
	sprintf(rmcmd, "ipcrm -Q %d", mqkey);
	system(rmcmd);

	// 打开消息队列
	mqcomm = auto_ptr<CMQCommu>(new CMQCommu(HEART_BEAT_MQID));
	
	// 清空历史消息
	int ret;
	do
	{
		ret = mqcomm->recv((struct msgbuf *)&promsg[0], sizeof(ProcMonMsg) - sizeof(long), 0);
	}while (ret > 0);
}

void CMonitorProcSrv::run()
{
	// 接收心跳包，刷新本地进程信息
	doRecv(0);
	// 检查进程组实际启动的进程数，如果小于配置minprocnum则fork创建，如果大于配置maxprocnum则kill销毁。正常范围内则检查各个进程是否超时。
	doCheck();
	//showProc();
}

void CMonitorProcSrv::killallproc()
{
	vector<CProcGroup>::iterator groupitor = procgroup.begin();
	while(groupitor != procgroup.end())
	{
		list<CProc> &procs = groupitor->proclist;
		list<CProc>::iterator procitor = procs.begin();
		while(procitor != procs.end())
		{
			kill(procitor->procid, SIGUSR1);
			procitor++;
		}
		
		groupitor++;
	}
}

void CMonitorProcSrv::doRecv(int msgtype)
{
	int ret;
	do
	{
		ret = mqcomm->recv((struct msgbuf *)&promsg[0], sizeof(ProcMonMsg) - sizeof(long), msgtype);

		if(ret > 0)
		{
			if(promsg[0].type == PROC_INFO_TICK)
			{
				ProcMsg &tinfo = promsg[0].body.tickmsg;
				CProc &proc = findProc(tinfo.groupid, tinfo.procid);
				if(proc.procid != -1)
				{
					proc.timestamp = tinfo.timestamp;
				}
				else
				{
					addProc(tinfo);
				}

			}
		}
		else
		{
			log_debug("receive heartbeat message over. ret=%d, errno=%d", ret, errno);
			cout << "receive heartbeat message over. ret="<< ret << ", errno="<< errno << endl;
		}
	}
	while (ret > 0);
}

void CMonitorProcSrv::doCheck()
{
	vector<CProcGroup>::iterator groupitor = procgroup.begin();
	while(groupitor != procgroup.end())
	{
		int currpronum = groupitor->proclist.size();
		int minprocnum = groupitor->groupinfo.minprocnum;
		int maxprocnum = groupitor->groupinfo.maxprocnum;

		if(currpronum < minprocnum)
		{
			ProcEvent event(PROCMON_EVENT_PROCDOWN, groupitor->groupinfo.groupid, -1);
			events.push_back(event);
		}
		else if(currpronum > maxprocnum)
		{
			ProcEvent event(PROCMON_EVENT_PROCUP, groupitor->groupinfo.groupid, -1);
			events.push_back(event);
		}

		list<CProc> &procs = groupitor->proclist;
		list<CProc>::iterator procitor = procs.begin();
		while(procitor != procs.end())
		{
			time_t now = time(NULL);
			if((now - procitor->timestamp) > 20)
			{
				ProcEvent event(PROCMON_EVENT_PROCDEAD, groupitor->groupinfo.groupid, procitor->procid);
				events.push_back(event);
			}
			procitor++;
		}
		
		groupitor++;
	}

	doEvent();
}

CProc &CMonitorProcSrv::findProc(int groupid, int procid)
{
	CProc ret;

	vector<CProcGroup>::iterator groupitor = procgroup.begin();
	while(groupitor != procgroup.end())
	{
		if(groupitor->groupinfo.groupid == groupid)
		{
			list<CProc> &procs = groupitor->proclist;
			list<CProc>::iterator procitor = procs.begin();
			while(procitor != procs.end())
			{
				if(procitor->procid == procid)
				{
					return *procitor;
				}
				procitor++;
			}
		}
		groupitor++;
	}

	return ret;
}

void CMonitorProcSrv::addProc(ProcMsg &promsg)
{
	CProc proc(promsg.procid, promsg.timestamp);

	vector<CProcGroup>::iterator groupitor = procgroup.begin();
	while(groupitor != procgroup.end())
	{
		if(groupitor->groupinfo.groupid == promsg.groupid)
		{
			list<CProc> &procs = groupitor->proclist;
			list<CProc>::iterator it = find(procs.begin(), procs.end(), proc);
			if(it == procs.end())
			{
				procs.insert(it, proc);
			}
			break;
		}
		groupitor++;
	}
}

void CMonitorProcSrv::delProc(int groupid, int procid)
{
	vector<CProcGroup>::iterator groupitor = procgroup.begin();
	while(groupitor != procgroup.end())
	{
		if(groupitor->groupinfo.groupid == groupid)
		{
			list<CProc> &procs = groupitor->proclist;
			CProc proc(procid, 0);
			list<CProc>::iterator procitor = remove(procs.begin(), procs.end(), proc);
			procs.erase(procitor, procs.end());
		}
		groupitor++;
	}
}

void CMonitorProcSrv::showProc()
{
	cout << "================ static group proc ================" << endl;
	vector<CProcGroup>::iterator groupitor = procgroup.begin();
	while(groupitor != procgroup.end())
	{
		{
			list<CProc> &procs = groupitor->proclist;
			list<CProc>::iterator procitor = procs.begin();
			cout << "procgroup " << groupitor->groupinfo.groupid << " contains proc list:";
			while(procitor != procs.end())
			{
				cout << procitor->procid << " ";
				procitor++;
			}
			cout << endl;
		}
		groupitor++;
	}
	cout << "================ static group proc ================" << endl;
}

void CMonitorProcSrv::doEvent()
{
	for(list<ProcEvent>::iterator itor = events.begin(); itor != events.end(); itor++)
	{
		int event = itor->eventid;
		int groupid = itor->groupid;
		int procid = itor->procid;

		if(event & PROCMON_EVENT_PROCDOWN)
		{
			for(vector<CProcGroup>::iterator groupitor = procgroup.begin(); groupitor != procgroup.end(); groupitor++)
			{
				if(groupitor->groupinfo.groupid == groupid)
				{
					CProcGroup &procgroup = *groupitor;
					int currpronum = procgroup.proclist.size();
					int minprocnum = procgroup.groupinfo.minprocnum;
					int diff = minprocnum - currpronum;

					char cmdbuf[256];
					snprintf(cmdbuf, sizeof(cmdbuf) - 1, "./%s", procgroup.groupinfo.execfile.c_str());
					
					for(int i=0; i<diff; i++)
					{
						log_debug("process heartbeat event. start new process:%s", procgroup.groupinfo.execfile.c_str());
						system(cmdbuf);
						usleep(12000);
					}
				}
			}
		}
		else if(event & PROCMON_EVENT_PROCUP)
		{
			vector<pair<int,int> > waitdel;

			for(vector<CProcGroup>::iterator groupitor = procgroup.begin(); groupitor != procgroup.end(); groupitor++)
			{
				if(groupitor->groupinfo.groupid == groupid)
				{
					CProcGroup &procgroup = *groupitor;
					int currpronum = procgroup.proclist.size();
					int maxprocnum = procgroup.groupinfo.maxprocnum;
					int diff = currpronum - maxprocnum;

					if(diff > 0)
					{
						list<CProc> &procs = procgroup.proclist;
						list<CProc>::iterator itor = procs.begin();
						advance(itor, maxprocnum);
						for(; itor != procs.end(); itor++)
						{
							log_debug("process heartbeat event. delete surplus process:%s", procgroup.groupinfo.execfile.c_str());
							waitdel.push_back(make_pair(groupid, itor->procid));
						}
					}
				}
			}

			for(int i=0; i<waitdel.size(); i++)
			{
				int groupid = waitdel.at(i).first;
				int procid  = waitdel.at(i).second;

				kill(procid, SIGUSR1);
				delProc(groupid, procid);
				usleep(12000);
				doRecv(procid);
			}
		}
		else if(event & PROCMON_EVENT_PROCDEAD)
		{
			kill(procid, SIGKILL);
			doRecv(procid);
			delProc(groupid, procid);

			auto procGroupIter = find_if(procgroup.begin(), procgroup.end(), [&](CProcGroup &pg)->bool{
 				if(pg.groupinfo.groupid == groupid) {
					return true;
				}
			});
			if(procGroupIter != procgroup.end())
			{
				log_debug("process heartbeat event. delete death process:%s", procGroupIter->groupinfo.execfile.c_str());
			}
		}
	}

	events.clear();
}
