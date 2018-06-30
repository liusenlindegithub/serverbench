#include <iostream>
#include <memory>

#include "message.h"
#include "../../netcomm/clientdata.h"
#include "../../netcomm/pollerobject.h"
#include "../../netcomm/client.h"
#include "../../netcomm/processcomm.h"

using namespace std;

extern "C"
int bench_handle_init(void *arg1, void *arg2)
{
	cout << "bench_handle_init" << endl;
	return 0;
}

extern "C"
int bench_handle_input(void *arg1, void *arg2)
{
	cout << "bench_handle_input, sizeof(dataheader)=" << sizeof(dataheader) << endl;

	CClientData *dataCache = (CClientData *)arg1;
	int pkglen = PackageProcess::haspackage(dataCache);

	return pkglen;
}

extern "C"
int bench_handle_route(void *arg1, void *arg2)
{
	return 1;
}

extern "C"
//int bench_handle_process(void *arg1, void *arg2)
int bench_handle_process(char *data, int datalen, int seq, void *arg)
{
	cout << "bench_handle_process" << endl;

	ProcessCommItem *commItem = dynamic_cast<ProcessCommItem *>((ProcessCommItem *)arg);

	if(data == NULL) {
		cout << "dataCache NULL Pointer..." << endl;
		exit(0);
	}

	if(commItem == NULL) {
		cout << "commitem NULL Pointer..." << endl;
		exit(0); 
	}
	
	// ·µ»Ø¿Í»§¶Ë
	shared_ptr<ShmProducer> producer = commItem->getProducer();
	if(producer) {
		producer->produce(data, datalen, seq);
		producer->sendNotify();
	}

	return 0;
}