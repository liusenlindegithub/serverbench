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
int bench_handle_process(void *arg1, void *arg2)
{
	cout << "bench_handle_process" << endl;

	CClientData *dataCache = (CClientData *)arg1;
	ProcessCommItem *commItem = dynamic_cast<ProcessCommItem *>((ProcessCommItem *)arg2);

	if(dataCache == NULL) {
		cout << "dataCache NULL Pointer..." << endl;
		exit(0);
	}

	if(commItem == NULL) {
		cout << "commitem NULL Pointer..." << endl;
		exit(0); 
	}

	int seq;
	shared_ptr<datapackage> response = PackageProcess::getPackage(dataCache, seq);
	if(response != NULL)
	{
		cout << "bench_handle_process data=" << response->getData() << endl;

		// ·µ»Ø¿Í»§¶Ë
		shared_ptr<ShmProducer> producer = commItem->getProducer();
		if(producer) {
			char *data = (char *)malloc(sizeof(int) + response->header.mDataLength);
			int datalen = response->header.mDataLength + sizeof(int);
			memcpy(data, &datalen, sizeof(int));
			memcpy(data + sizeof(int), response->getData(), response->header.mDataLength);

			producer->produce(data, datalen, seq);
			producer->sendNotify();

			delete data;
		}
	}
	else
	{
		cout << "bench_handle_process NULL FAILED" << endl;
	}

	return 0;
}