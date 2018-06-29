#ifndef _FIFO_COMM_H
#define _FIFO_COMM_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <string>
using namespace std;

class FifoComm
{
public:
	FifoComm(int _key);
	int createFifo();
	int getFd();
	int fifoWrite(const char *buff, int size);
	int fifoRead(char *buff, int buffsize);
private:
	string getFifoPath(int key);

private:
	int fd;
	int key;
};

#endif