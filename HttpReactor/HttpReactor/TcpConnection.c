#include "TcpConnection.h"
#include "Buffer.h"
#include "Channel.h"
#include <stdio.h>
#include <stdlib.h>
#include "Channel.h"
#include "Buffer.h"


int processRead(void* arg)
{
	struct TcpConnection* conn = (struct TcpConnection*)arg;
	int fd = conn->channel->fd;
	int count = bufferSocketData(conn->readBuf, fd);
	if (count > 0)
	{
		parseHttpRequest(conn->request, conn->response, conn->readBuf, conn->writeBuf, fd);
	}
	return 0;
}

int processWrite(void* arg)
{
	return 0;
}


void TcpConnectionInit(struct EventLoop* evLoop, int cfd)
{
	struct TcpConnection* conn = (struct TcpConnection*)malloc(sizeof(struct TcpConnection));
	conn->evLoop = evLoop;
	conn->readBuf = BufferInit(40960);
	conn->writeBuf = BufferInit(40960);
	conn->channel = ChannelInit(cfd, readEvent, processRead, processWrite, NULL, conn);
	eventLoopAddTask(conn->evLoop, conn->channel, ADD);
	conn->request = HttpRequsetInit();
	conn->response = HttpResponseInit();
}

void tcpConnectionDestroy(struct TcpConnection* conn)
{
	if (conn != NULL)
	{
		if (conn->readBuf && 0 == bufferReadableSize(conn->readBuf) &&
			conn->writeBuf && 0 == bufferReadableSize(conn->writeBuf))
		{
			
		}
	}
}
