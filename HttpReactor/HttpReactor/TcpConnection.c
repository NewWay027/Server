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
	//接收数据
	int count = bufferSocketData(conn->readBuf, fd);
	if (count > 0)
	{
#ifndef MSG_SEND_AUTO
		writeEventEnable(conn->channel, true);
		eventLoopAddTask(conn->evLoop, conn->channel, MOD);
#endif 
		bool flag = parseHttpRequest(conn->request, conn->response, conn->readBuf, conn->writeBuf, fd);
		if (!flag)
		{
			char* errMsg = "Http/1.1 400 Bad Request\r\n\r\n";
			bufferAppendString(conn->writeBuf, errMsg);
		}
	}
#ifdef MSG_SEND_AUTO
	//断开连接
	eventLoopAddTask(conn->evLoop, conn->channel, DEL);
#endif 
	return 0;
}

int processWrite(void* arg)
{
	struct TcpConnection* conn = (struct TcpConnection*)arg;
	int count = bufferSendData(conn->writeBuf, conn->channel->fd);
	if (count > 0)
	{
		//数据全部被发送出去
		if (0 == bufferReadableSize(conn->writeBuf))
		{
			//删除节点
			eventLoopAddTask(conn->evLoop, conn->channel, DEL);
		}
	}
	return 0;
}

int tcpConnectionDestroy(void * arg)
{
	struct TcpConnection* conn = (struct TcpConnection*)arg;
	if (conn != NULL)
	{
		if (conn->readBuf && 0 == bufferReadableSize(conn->readBuf) &&
			conn->writeBuf && 0 == bufferReadableSize(conn->writeBuf))
		{
			channelDestroy(conn->evLoop, conn->channel);
			bufferDestroy(conn->readBuf);
			bufferDestroy(conn->writeBuf);
			HttpRequestDestroy(conn->request);
			HttpResponseDestroy(conn->response);
			free(conn);
		}
	}
}

void TcpConnectionInit(struct EventLoop* evLoop, int cfd)
{
	struct TcpConnection* conn = (struct TcpConnection*)malloc(sizeof(struct TcpConnection));
	conn->evLoop = evLoop;
	conn->readBuf = BufferInit(10240);
	conn->writeBuf = BufferInit(10240);
	conn->channel = ChannelInit(cfd, readEvent, processRead, processWrite, tcpConnectionDestroy, conn);
	eventLoopAddTask(conn->evLoop, conn->channel, ADD);
	conn->request = HttpRequsetInit();
	conn->response = HttpResponseInit();
}


