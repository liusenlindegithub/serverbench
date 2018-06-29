#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <exception>
#include <string>
#include <iostream>

#include "ByteBuffer.h"

using namespace std;

ByteBuffer::ByteBuffer()
{
	buff = NULL;
	cursor = NULL;
	capacity = 0;
	size = 0;
}

ByteBuffer::~ByteBuffer()
{
	if(NULL != buff)
	{
		free(buff);
	}
}

void ByteBuffer::ExpandBuff(int expandSize) throw(exception)
{
	int newsize = ((capacity + expandSize) < 2*capacity) ? 2*capacity : (capacity + expandSize);
	char *addr = (char *)realloc(buff, newsize);
	if(addr == NULL)
	{
		throw exception();
	}
	else 
	{
		buff = addr;
		cursor = buff;
		capacity = newsize;
	}
}

template <class Type>
void ByteBuffer::WriteType(Type t) throw(exception)
{
	if((capacity - size) < sizeof(t))
	{
		ExpandBuff(sizeof(t));
	}

	*(Type *)(buff + size) = t;

	size += sizeof(t);
}

void ByteBuffer::WriteChar(char c) throw(exception)
{
	WriteType<char>(c);
}

void ByteBuffer::WriteShort(short int si) throw(exception)
{
	WriteType<short int>(si);
}

void ByteBuffer::WriteInt(int i) throw(exception)
{
	WriteType<int>(i);
}

/**
向缓存中写入len个字节
**/
void ByteBuffer::WriteBytes(char *bytes, int offset, int len) throw(exception)
{
	if((capacity - size) < len)
	{
		ExpandBuff(len);
	}

	char *wp = buff + size;
	memcpy(wp, bytes, len);

	size += len;
}

void ByteBuffer::PrintBuff()
{
	for(int i=0; i< size; i++)
	{
		printf("0X%x ", buff[i]);
	}
	printf("\n");
}

char * ByteBuffer::GetBuff(char *&out, int &len)
{
	out = buff;
	len = size;
	return out;
}

void ByteBuffer::wrap(char *bytes, int offset, int len)
{
	if(NULL != buff)
	{
		free(buff);
		buff = NULL;
		capacity = 0;
		size = 0;
	}

	WriteBytes(bytes, offset, len);

	cursor = buff;
}

template <class Type>
void ByteBuffer::ReadType(Type &t) throw(exception)
{
	if((size - (cursor - buff)) < sizeof(t))
	{
		throw exception();
	}
	else
	{
		if(NULL == cursor) cursor = buff;
		t = *(Type *)cursor;
		cursor += sizeof(t); 
	}
}

void ByteBuffer::ReadChar(char &c) throw(exception)
{
	ReadType<char>(c);
}

void ByteBuffer::ReadShort(short &si) throw(exception)
{
	ReadType<short>(si);
}

void ByteBuffer::ReadInt(int &i) throw(exception)
{
	ReadType<int>(i);
}

/**
从缓冲中读取len个字节到bytes中
**/
void ByteBuffer::ReadBytes(char *bytes, int len) throw(exception)
{
	if((size - (cursor - buff)) < len)
	{
		throw exception();
	}
	else
	{
		if(NULL == cursor) cursor = buff;
		memcpy(bytes, cursor, len);
		cursor += len;
	}
}