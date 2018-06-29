#include <iostream>
#include <memory>

#include "msg.pb.h"
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
	cout << "dataCache->mReadPos=" << dataCache->mReadPos << endl;
	cout << "dataCache->mWritePos=" << dataCache->mWritePos << endl;

	return dataCache->mWritePos - dataCache->mReadPos;
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

	cout << "22222dataCache->mReadPos=" << dataCache->mReadPos << endl;
	cout << "22222dataCache->mWritePos=" << dataCache->mWritePos << endl;

	if(dataCache == NULL) {
		cout << "dataCache NULL Pointer..." << endl;
		exit(0);
	}

	if(commItem == NULL) {
		cout << "commitem NULL Pointer..." << endl;
		exit(0);
	}

	char *data = dataCache->mData;
	int seq = *(int *)data;
	data += sizeof(int);

	//将信息从字符串中反格式化出来（读操作）
    demo::msg msg_encoding;
    msg_encoding.ParseFromArray(data, dataCache->mWritePos - sizeof(int));

	cout << "--------->msg type:" << msg_encoding.msgtype() << endl;
    cout << "--------->msg info:" << msg_encoding.msginfo() << endl;
    cout << "--------->msg from:" << msg_encoding.msgfrom() << endl;

	// 不支持对UDP做返回

	return 0;
}