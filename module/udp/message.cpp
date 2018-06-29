#include "message.h"

datapackage::datapackage():mData(NULL)
{
	
}

datapackage::~datapackage()
{
	if(mData)
	{
		delete mData;
	}
}

const char * datapackage::getData() const
{
	return mData;
}

/**
参数：EPOLLIN接收到本地缓冲区数据
返回值：
如果存在数据包，则返回业务数据包长度;
如果没有数据包，则返回0;
**/
int PackageProcess::haspackage(CClientData *dataCache)
{
	int pkglen = 0;
	int datalen = 0;
	char *data = dataCache->getReadbleData(datalen);

	if(datalen < sizeof(dataheader))
	{
		return 0;
	}
	else
	{
		dataheader *header = (dataheader *)data;
		if(header->mDataLength > datalen - sizeof(dataheader))
		{
			return 0;
		}
		else
		{
			pkglen = sizeof(dataheader) + header->mDataLength;
			return pkglen;
		}
	}
}

shared_ptr<datapackage> PackageProcess::getPackage(CClientData *dataCache)
{
	shared_ptr<datapackage> pkg(NULL);
	int pkglen = haspackage(dataCache);
	if(pkglen > 0)
	{
		pkg = shared_ptr<datapackage>(new datapackage());
		int datalen = 0;
		char *data = dataCache->getReadbleData(datalen);

		pkg->header = *(dataheader *)data;
		pkg->mData = (char *)malloc(pkg->header.mDataLength + 2);
		memcpy(pkg->mData, data + sizeof(dataheader), pkg->header.mDataLength);
	}

	return pkg;
}