#include <memory>
using namespace std;

#include "defaultproxy.h"

int main(int argc, char *argv[])
{
	daemon(1, 1);

	shared_ptr<CServerBase> proxy(new CDefaultProxy());
	proxy->run();

	return 0;
}