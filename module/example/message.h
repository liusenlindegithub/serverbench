#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include "../../netcomm/clientdata.h"
#include "../../netcomm/client.h"

#pragma pack(1)
typedef struct DataHeader
{
	int mDataLength;
}dataheader;
#pragma pack()


class datapackage : public NoCopyable 
{
public:
	friend class CClientData;
	friend class CClient;

	datapackage();
	virtual ~datapackage();
	const char * getData() const;
	
public:
	dataheader  header;
	char *mData;
};

class PackageProcess
{
public:
	static int haspackage(CClientData *dataCache);
	static shared_ptr<datapackage> getPackage(CClientData *dataCache, int &seq);
};


#endif