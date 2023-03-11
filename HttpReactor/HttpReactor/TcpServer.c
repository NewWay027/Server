#include "TcpServer.h"
#include "ThreadPool.h"
#include "Channel.h"
#include "TcpConnection.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>


struct TcpServer* TcpServerInit(unsigned int port, int threadNum)
{
	struct TcpServer* tcpServer = (struct TcpServer*)malloc(sizeof(struct TcpServer));
	tcpServer->threadNum = threadNum;
	tcpServer->listener = ListenerInit(port);
	tcpServer->mainLoop = eventLoopInit("MainLoop");
	tcpServer->pool = ThreadPoolInit(tcpServer->mainLoop, threadNum);
	return tcpServer;
}

struct Listener* ListenerInit(unsigned int port)
{
	struct Listener* listener = (struct Listener*)malloc(sizeof(struct Listener));
	int lfd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == lfd)
	{
		perror("socket");
	}
	int opt = 1;
	int ret = setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if (-1 == ret)
	{
		perror("setsockopt");
	}

	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	ret = bind(lfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	if (-1 == ret)
	{
		perror("bind");
	}

	ret = listen(lfd, 128);
	if (-1 == ret)
	{
		perror("listen");
	}
	listener->fd = lfd;
	listener->port = port;
	return listener;
}

int acceptConnection(void* arg)
{
	struct TcpServer* tcpServer = (struct TcpServer*)arg;
	int cfd = accept(tcpServer->listener->fd, NULL, NULL);
	struct EventLoop * evLoop = takeOneEventLoop(tcpServer->pool);
	TcpConnectionInit(evLoop, cfd);
	return 0;
}


void TcpServerRun(struct TcpServer* tcpServer)
{
	//1.启动线程池
	ThreadPoolRun(tcpServer->pool);
	//2.新建一个用于监听的套接字Channel
	struct Channel* channel = ChannelInit(tcpServer->listener->fd, readEvent, acceptConnection, NULL, NULL, tcpServer);
	eventLoopAddTask(tcpServer->mainLoop, channel, ADD);
	//启动反应堆循环检测
	eventLoopRun(tcpServer->mainLoop);
}
