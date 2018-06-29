#include <unistd.h>
#include <libgen.h>

#include "fifocomm.h"
#include "../utils/log/log.h"

FifoComm::FifoComm(int _key)
{
	key = _key;
	fd = -1;
}

/**
成功创建fifo返回0， 失败返回<0
**/
int FifoComm::createFifo()
{
	int res;
	string fifopath = getFifoPath(key);

	if(access(fifopath.c_str(), F_OK) == -1)
	{
		res = mkfifo(fifopath.c_str(), 0777);
		if(res != 0)
		{
			log_debug("createFifo failed.");
			return -1;
		}
	}

	fd = open(fifopath.c_str(), O_RDWR | O_NONBLOCK, 0777);
	if(fd == -1)
	{
		log_debug("open fifo failed.");
		return -2;
	}
log_debug("~~~~~fifo fd=%d, pid=%d", fd, getpid());
	return 0;
}

int FifoComm::getFd()
{
	return fd;
}

int FifoComm::fifoWrite(const char *buff, int size)
{
	return write(fd, buff, size);
}

int FifoComm::fifoRead(char *buff, int buffsize)
{
	return read(fd, buff, buffsize);
}

string FifoComm::getFifoPath(int key)
{
	char fifopath[256];
	readlink("/proc/self/exe", fifopath, 256);
	dirname(fifopath);

	char fifofile[128] = {0};
	snprintf(fifofile, sizeof(fifofile), "/.fifofile_%d", key);
	strcat(fifopath, fifofile);

	return string(fifopath);
}