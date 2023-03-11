#pragma once

#include "EventLoop.h"
#include "ThreadPool.h"

struct Listener 
{
	unsigned int port;
	int fd;
};
struct TcpServer
{
	struct EventLoop* mainLoop;
	struct Listener* listener;
	struct ThreadPool* pool;
	int threadNum;
};

//初始化tcp服务器
struct TcpServer* TcpServerInit(unsigned int port, int threadNum);
//初始化Listener
struct Listener* ListenerInit(unsigned int port);
//启动tcp服务器
void TcpServerRun(struct TcpServer * tcpServer);