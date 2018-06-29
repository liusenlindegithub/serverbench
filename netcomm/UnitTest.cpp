#include <unistd.h>
#include <iostream>
#include <vector>

using namespace std;

#include "cshmcommu.h"

int main()
{
	CShmMQ shm;
	shm.createShm(1234, 2048);


	int pid = fork();
	if(pid < 0)
	{
		cout << "fork failed." << endl;
		return 0;
	}
	else if(pid > 0)
	{
		vector<string> testmsg;
		testmsg.push_back("a");
		testmsg.push_back("ab");
		testmsg.push_back("abc");
		testmsg.push_back("abcd");
		testmsg.push_back("abcde");
		testmsg.push_back("abcdef");
		testmsg.push_back("abcdefg");
		testmsg.push_back("abcdefgh");
		testmsg.push_back("abcdefghi");
		testmsg.push_back("abcdefghij");

		unsigned int count = 0;
		while(true)
		{
			count++;
			int i = count % 10;
			CSHM_Q_RESULT ret = shm.enqueue(testmsg[i].c_str(), i+1, count);
			//cout << "--> enquue: hello" << ", ret=" << ret << endl;
			if(ret != CSHM_ENQUEUE_SUCCESS)
			{
				usleep(100);
			}
		}
	}
	else
	{
		unsigned int datalen;
		int seq;
		while(true)
		{
			char buff[128] = {0};
			CSHM_Q_RESULT ret = shm.dequeue((void *)buff, (unsigned int)128, datalen, seq);

			if(ret != CSHM_DEQUEUE_SUCCESS)
			{
				//cout << "<-- dequue:error" << ", ret=" << ret << endl;
				usleep(100);
			}
			else
			{
				//cout << "<-- dequue:" << ", buff=" << buff << endl;
			}
		}
	}
}