#ifndef _CLIENT_DATA_H_
#define _CLIENT_DATA_H_

#include <memory>
using namespace std;

#include "../comm/nocopyable.h"

#define CLIENT_DATA_BUFF_SIZE 1024



/**
功能：
网络I0可读时写入客户端数据，作为应用层客户端数据缓存。
**/
class CClient;
class CClientData
{
public:
	friend class datapackage;
	friend class CClient;

	CClientData();
	virtual ~CClientData();
	int write(char *buff, int size);
/*
	shared_ptr<datapackage> getPackage();
	bool haspackage();
	void fillpackage(shared_ptr<datapackage> package);
*/
	
	void alignPosition();
	void alignPosition(int prolen);
	char *getReadbleData(int &len);
	
public:
	char *mData;
	int  mWritePos;    //写入位置
	int  mReadPos;     //读取位置
	int  mCapacity;    //总共大小

	int expandeSpace(int newsize);
	int revertSpace();
};

#endif