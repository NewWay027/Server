#pragma once

#include "EventLoop.h"
#include "Buffer.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

struct TcpConnection 
{
	struct EventLoop* evLoop;
	struct Channel* channel;
	struct Buffer* readBuf;
	struct Buffer* writeBuf;
	struct HttpRequest* request;
	struct HttpResponse* response;
};

//≥ı ºªØTcpConnection
void TcpConnectionInit(struct EventLoop* evLoop, int cfd);
void tcpConnectionDestroy(struct TcpConnection * conn);