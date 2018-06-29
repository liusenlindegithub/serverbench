#include <stdlib.h>
#include <iostream>

#include <unistd.h>
#include <poll.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>

#include "msg.pb.h"

#include <stdio.h> 
#include <string.h> 
#include <errno.h> 
#include <sys/socket.h> 
#include <resolv.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <openssl/ssl.h> 
#include <openssl/err.h> 

#include <chrono>
#include <ctime>

using namespace std;

/**
测试远程服务器 115.28.54.202 的收发，对应server的 module/example 应用
**/
int test_tcp_echo()
{
	const char *ip = "115.28.54.202";
	int port = 8881;
	
	struct sockaddr_in address;
	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &address.sin_addr);
	address.sin_port = htons(port);
	
	int sockfd = socket(PF_INET, SOCK_STREAM, 0);

	if(connect(sockfd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		cout << "connection failed." << endl;
		close(sockfd);
		return 0;
	}
	
	cout << "connection returned. errno=" << errno << endl;

	// 一发一收，并统计每100个请求收发服务的处理时长，观察服务器性能是否会随着时间便弱。
	int processCounter = 0;
	auto timeStart = std::chrono::system_clock::now();
	auto timeEnd = std::chrono::system_clock::now();
	while(true)
	{
		if(processCounter == 0) 
		{
			timeStart = std::chrono::system_clock::now();
		}

		const char *msg = "hello";
		int len = strlen(msg);
		write(sockfd, &len, sizeof(len));
		write(sockfd, msg, len);
		//cout << "send: hello" << endl;

		char buff[1024] = {0};
		read(sockfd, buff, sizeof(buff));
		//cout << "recv len=" << *(int *)buff << endl;
		//cout << "recv:" << buff + sizeof(int) << endl;
		
		processCounter++;
		if(processCounter == 100) 
		{
			processCounter = 0;
			timeEnd = std::chrono::system_clock::now();
			std::chrono::duration<double> elapsedTimeSeconds = timeEnd - timeStart;
			cout << "send recv 100 request elapsed " << elapsedTimeSeconds.count() << " seconds." << endl;
		}
	}

	return 1;
}

int test_tcp_spp()
{
	const char *ip = "192.168.111.128";
	int port = 5575;
	
	struct sockaddr_in address;
	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &address.sin_addr);
	address.sin_port = htons(port);
	
	int sockfd = socket(PF_INET, SOCK_STREAM, 0);

	if(connect(sockfd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		cout << "connection failed." << endl;
		close(sockfd);
		return 0;
	}
	
	cout << "connection returned. errno=" << errno << endl;

	basic::DemoMsg demoMsg;
	basic::Header *p_header = demoMsg.mutable_header();
	p_header->set_msgid(basic::DEMO_MSG_ID);
	p_header->set_msgtype(basic::MSGTYPE_WETEST);
	demoMsg.set_field("demo");

	char msg[1024];
	demoMsg.SerializeToArray(msg,1024);
	int len = demoMsg.ByteSize();
	uint32_t proto_len = len;
	uint32_t protol_id = basic::DEMO_MSG_ID;
	write(sockfd, &proto_len, sizeof(uint32_t));
	write(sockfd, &protol_id, sizeof(uint32_t));
	write(sockfd, msg, len);
	cout << "send: len=" << len << endl;
	cout << "send: hello" << endl;

	char buff[1024] = {0};
	read(sockfd, buff, sizeof(buff));
	cout << "recv len=" << strlen(buff) << endl;
	cout << "recv:" << buff << endl;
	sleep(1);

	return 1;
}


int test_udp_echo()
{
	const char *ip = "192.168.111.128";
	int port = 6674;
	
	struct sockaddr_in address;
	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &address.sin_addr);
	address.sin_port = htons(port);

	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	while(true)
	{
		char *msg = "hello";
		int len = strlen(msg);
		cout << "send: len=" << len << endl;
		
		int n=sendto(sockfd, msg, len , 0, (struct sockaddr *)&address, sizeof(address));
		if(n>0) {
			cout << "send: msg" << endl;
		} else {
			cout << "send: failed. errno=" << errno << endl;
		}

		char buff[2048];
		recvfrom(sockfd, buff, 2048, 0, (struct sockaddr *)&address, (socklen_t*)&len);
		cout << "recv len=" << *(int *)buff << endl;
		cout << "recv:" << buff + sizeof(int) << endl;
		sleep(1);
		break;
	}

	return 1;
}


/***
测试https请求。
需要预先设置好apache 的ssl配置。
***/
#define MAXBUF 1024 

void ShowCerts(SSL * ssl) 
{
    X509 * cert;
    char * line;
    cert = SSL_get_peer_certificate(ssl);
    if (cert != NULL) 
    {
        printf("数字证书信息:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("证书: %s\n", line);
        free(line);
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("颁发者: %s\n", line);
        free(line);
        X509_free(cert);
    } 
    else 
    {
        printf("无证书信息！\n");
    }
} 

int test_https_request()
{
	int sockfd, len;
    struct sockaddr_in dest;
    char buffer[MAXBUF + 1];
    SSL_CTX * ctx;
    SSL * ssl;
    
    /* SSL 库初始化*/
    SSL_library_init();
    /* 载入所有SSL 算法*/
    OpenSSL_add_all_algorithms();
    /* 载入所有SSL 错误消息*/
    SSL_load_error_strings();
    /* 以SSL V2 和V3 标准兼容方式产生一个SSL_CTX ，即SSL Content Text */
    ctx = SSL_CTX_new(SSLv23_client_method());
    if (ctx == NULL) 
    {
        ERR_print_errors_fp(stdout);
        exit(1);
    }
    /* 创建一个socket 用于tcp 通信*/
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
        perror("Socket");
        exit(errno);
    }
    printf("socket created\n");
    /* 初始化服务器端（对方）的地址和端口信息*/
    bzero( &dest, sizeof(dest));
    dest.sin_family = AF_INET;
    //设置连接的端口
    dest.sin_port = htons(443);
    //设置连接的IP地址
    if (inet_aton("50.56.19.116", (struct in_addr * ) &dest.sin_addr.s_addr) == 0) 
    {
        perror("50.56.19.116");
        exit(errno);
    }
    printf("address created\n");
    /* 连接服务器*/
    if (connect(sockfd, (struct sockaddr * ) &dest, sizeof(dest)) != 0) 
    {
        perror("Connect ");
        exit(errno);
    }
    printf("server connected\n");
    /* 基于ctx 产生一个新的SSL */
    ssl = SSL_new(ctx);
    /* 将新连接的socket 加入到SSL */
    SSL_set_fd(ssl, sockfd);
    /* 建立SSL 连接*/
    if (SSL_connect(ssl) == -1) 
    {
        ERR_print_errors_fp(stderr);
    }
    else 
    {
        printf("Connected with %s encryption\n", SSL_get_cipher(ssl));
        ShowCerts(ssl);
    }

	string request = "GET https://www.fiddler2.com/UpdateCheck.aspx?isBeta=False HTTP/1.1\r\n";
	request += "User-Agent: Fiddler/4.6.20171.26113 (.NET 4.6.2; WinNT 10.0.14393.0; zh-CN; 4xAMD64; Auto Update; Full Instance; Extensions: APITesting, AutoSaveExt, EventLog, Geoedge, HostsFile, RulesTab2, SAZClipboardFactory, SimpleFilter, Timeline)\r\n";
	request += "Pragma: no-cache\r\n";
	request += "Host: www.fiddler2.com\r\n";
	request += "Connection: close\r\n";
	request += "\r\n";
	request += "\r\n";


	/* 发消息给服务器*/
    len = SSL_write(ssl, request.c_str(), request.length());
    if (len < 0) 
    {
        printf("消息'%s'发送失败！错误代码是%d，错误信息是'%s'\n",  request.c_str(), errno, strerror(errno));
    }
    else 
    {
        printf("消息'%s'发送成功，共发送了%d 个字节！\n", request.c_str(), len);
    }


    /* 接收对方发过来的消息，最多接收MAXBUF 个字节*/
    bzero(buffer, MAXBUF + 1);
    /* 接收服务器来的消息*/
    len = SSL_read(ssl, buffer, MAXBUF);
    if (len > 0) 
    {
        printf("接收消息成功:'%s'，共%d 个字节的数据\n", buffer, len);
    }
    else 
    {
        printf("消息接收失败！错误代码是%d，错误信息是'%s'\n", errno, strerror(errno));
        goto finish;
    }

    finish:
    /* 关闭连接*/
    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(sockfd);
    SSL_CTX_free(ctx);
    return 0;
}


int main()
{
	test_tcp_echo();

	return 0;
}
