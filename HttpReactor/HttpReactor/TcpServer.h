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

//��ʼ��tcp������
struct TcpServer* TcpServerInit(unsigned int port, int threadNum);
//��ʼ��Listener
struct Listener* ListenerInit(unsigned int port);
//����tcp������
void TcpServerRun(struct TcpServer * tcpServer);