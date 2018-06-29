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

===>  在 proxy的 bench_handle_input() 中调用
客户端按照 datapackage 的格式传包
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


/**
参数：worker从share mem 中读取客户端请求数据，包装成CClientData对象
返回值：
datapackage 的指针

===>  在 worker 的 bench_handle_process() 中调用
客户端按照 datapackage 的格式传包，服务端也按照 datapackage 的格式返回 
**/
shared_ptr<datapackage> PackageProcess::getPackage(CClientData *dataCache, int &seq)
{
	// 读取客户端数据
	int datalen = 0;
	char *data = dataCache->getReadbleData(datalen);

	seq = *(int *)data;
	data += sizeof(int);
	datalen -= sizeof(int);

	// 构造返回包
	shared_ptr<datapackage> resp(new datapackage());
	resp->header = *(dataheader *)data;
	resp->mData = (char *)malloc(resp->header.mDataLength + 2); // 这是很不好的做法，每个请求处理都会用malloc分配空间，导致内存碎片。
	memcpy(resp->mData, data + sizeof(dataheader), resp->header.mDataLength);

	// 返回
	return resp;
}