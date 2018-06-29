#include <string.h>

#include "clientdata.h"
#include "../utils/log/log.h"


CClientData::CClientData()
{
	mCapacity = CLIENT_DATA_BUFF_SIZE;
	mReadPos = 0;
	mWritePos = 0;
	mData = (char *)malloc(mCapacity);
}

CClientData::~CClientData()
{
	log_debug("----CClientData::~CClientData()");
	if(mData)
	{
		free(mData);
	}
}
/*
bool CClientData::haspackage()
{
	int datasize = mWritePos - mReadPos;
	//log_debug("----mCapacity=%d, datasize=%d", mCapacity, datasize);
	if(datasize < sizeof(dataheader))
	{
		return false;
	}
	else
	{
		dataheader *header = (dataheader *)mData;
		if(header->mDataLength > datasize - sizeof(dataheader))
		{
			return false;
		}
		else
		{
			return true;
		}
	}
}

void CClientData::fillpackage(shared_ptr<datapackage> package)
{
	package->header = *(dataheader *)mData;
	int datalen = package->header.mDataLength;

	mReadPos += sizeof(dataheader);

	package->mData = (char *)malloc(datalen + 2);
	log_debug("------%p", package->mData);
	if(package->mData)
	{
		memcpy(package->mData, mData + mReadPos, datalen);
		mReadPos += datalen;
	}
	else
	{
		log_error("malloc failed.");
	}
}

shared_ptr<datapackage> CClientData::getPackage()
{
	shared_ptr<datapackage> package(NULL);
	if(haspackage())
	{
		package = shared_ptr<datapackage>(new datapackage());
		log_debug("-----pakcage=%p", (datapackage *)package.get());
		fillpackage(package);
	}
	
	return package;
}
*/
void CClientData::alignPosition()
{
	memcpy(mData, mData + mReadPos, mWritePos - mReadPos);
	mWritePos = mWritePos - mReadPos;
	mReadPos = 0;

	revertSpace();
}

void CClientData::alignPosition(int prolen)
{
	mReadPos += prolen;
	alignPosition();
}

char * CClientData::getReadbleData(int &len)
{
	len = mWritePos - mReadPos;
	return mData+mReadPos;
}

int CClientData::write(char *buff, int size)
{
	int leftspace = mCapacity - mWritePos;
	
	if(leftspace < size)
	{
		int ret = expandeSpace(mWritePos + size + 2); //多预留一个空间
		if(ret != 0)
		{
			return -1;
		}
	}

	memcpy(mData + mWritePos, buff, size);
	mWritePos = mWritePos + size;
	return 0;
}

int CClientData::expandeSpace(int newsize)
{
	mData = (char *)realloc(mData, newsize);
	if(mData)
	{
		mCapacity = newsize;
		return 0;
	}

	return -1;
}

int CClientData::revertSpace()
{
	// 缩减扩张的缓存，减少内存压力
	if(mCapacity > CLIENT_DATA_BUFF_SIZE && mWritePos < CLIENT_DATA_BUFF_SIZE)
	{
		log_debug("need test!!");
		mData = (char *)realloc(mData, CLIENT_DATA_BUFF_SIZE);
		if(mData)
		{
			mCapacity = CLIENT_DATA_BUFF_SIZE;
		}
		else
		{
			log_error("cannot relloc space!");
			return -1;
		}
	}

	return 0;
}