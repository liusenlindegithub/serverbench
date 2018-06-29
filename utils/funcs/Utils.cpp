#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#include <algorithm>

#include "Utils.h"

using namespace std;

int Utils::mypopen(char *cmd, char type)
{
	int pipefd[2];           //管道描述符
	int pid_t;               //进程描述符

	if(type !='r' && type != 'w')
	{
		printf("myopen() flag error/n");
		return NULL;
	}

	if(pipe(pipefd)<0)        //建立管道
	{
		printf("myopen() pipe create error/n");
		return NULL;
	}
	
	pid_t=fork();             //建立子进程

	if(pid_t < 0) 
		return NULL;

	if(0 == pid_t)            //子进程中......
	{
		if(type == 'r')
		{
			close(pipefd[0]);               //此函数返回文件指针是用于读，因此，在子进程中应该将结果写入管道，这样父进程返回的文件指针才能读，所以这里将读端关闭
			dup2(pipefd[1],STDOUT_FILENO);  //exec函数的执行结果将会通过标准输出写到控制台上，但这里我们不需要在控制台输出，而是需要将结果返回，因此将标准输出重定向到管道写端
			close(pipefd[1]);		

		}
		else{
			close(pipefd[1]);
			dup2(pipefd[0],STDIN_FILENO);
			close(pipefd[0]);
		}
		char *argv[] = {cmd,NULL};	
		if(execl("/bin/bash", "/bin/bash", "-c", cmd, NULL)<0)          //用exec族函数执行命令
			return NULL;
	}
	
	wait(0);                                //等待子进程返回

	if(type=='r'){
		close(pipefd[1]);
		return pipefd[0];	//由于程序需要返回的参数是文件指针，因此需要用fdopen函数将描述符打开，其返回值为相应的文件指针	
	}else{
		close(pipefd[0]);
		return pipefd[1];
	}
}

void Utils::delrn(char *str)
{
	if(str != NULL)
	{
		int len = strlen(str);
		int r_index = 0;
		int n_index = 0;
		int index;

		r_index = (str[len - 1] == '\r')?len -1:0;
		n_index = (str[len - 1] == '\n')?len -1:0;

		if(r_index == 0 && len >= 2) 
		{
			r_index = (str[len - 2] == '\r')?len -2:0;
		}

		if(n_index == 0 && len >= 2) 
		{
			n_index = (str[len - 2] == '\n')?len -2:0;
		}

		if(r_index != 0 && n_index != 0)
		{
			index = min(r_index, n_index);
		}
		else if(r_index == 0 && n_index != 0)
		{
			index = n_index;
		}
		else if(r_index != 0 && n_index == 0)
		{
			index = r_index;
		}
		else
		{
			index = 0;
		}

		if(index != 0)
		{
			str[index] = '\0';
		}
	}
}