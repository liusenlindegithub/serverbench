#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>

#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>

#include "../netcomm/csocket.h"
#include "../utils/funcs/Utils.h"

/**
一收一发服务，接收执行相应命令。
**/
int main()
{
	daemon(1, 1);

	char workdir[1024] = {0};
	chroot("/");
	chdir("/");
	getcwd(workdir, 1024);
	printf("-----workdir:%s\n", workdir);


	int lfd = CSocket::create();
	if(lfd > 0)
	{
		int sendbuf = 10*1024;
		int ret0 = setsockopt(lfd, SOL_SOCKET, SO_SNDBUF, &sendbuf, sizeof(int));
		
		int bufsize = -1;
		int buflen;
		int ret = getsockopt(lfd, SOL_SOCKET, SO_SNDBUF, &bufsize, (socklen_t *)&buflen);

		CSocket::set_reuseaddr(lfd);

		if(CSocket::bind(lfd, 8888) == 0)
		{
			if(CSocket::listen(lfd) == 0)
			{
				
			}
		}
	}

	struct sockaddr_in peer;
	socklen_t peersize = sizeof(struct sockaddr_in);
	int newfd = ::accept(lfd, (struct sockaddr *)&peer, &peersize);

	while(true)
	{
		char buff[10240] = {0};
		read(newfd, buff, 1024);
		printf("read:%s", buff);
		Utils::delrn(buff);
		int fd = Utils::mypopen(buff, 'r');

		printf("-------\n");
		while(true)
		{
			int size = read(fd, buff, 10240);

			if(size > 0)
			{
				write(newfd, buff, size);
			}
			else
			{
				break;
			}
		}

		close(fd);
	}

	return 0;
}
