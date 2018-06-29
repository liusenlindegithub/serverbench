#include <unistd.h>

#include "defaultctl.h"

int main()
{
	//创建后台进程
	if(daemon(1,1))
	{
		perror("daemon");
		return -1;
	}

	CDdefaultControl control;
	control.run();
}