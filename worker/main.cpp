#include <unistd.h>
#include <memory>
using namespace std;

#include "defaultworker.h"

int main(int argc, char *argv[])
{
	daemon(1, 1);

	shared_ptr<CServerBase> worker(new CDefaultWorker());
	worker->run();

	return 0;
}