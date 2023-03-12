#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include "ChannelMap.h"
#include "EventLoop.h"


struct EventLoop* eventLoopInit()
{
	return eventLoopInitEx(NULL);
}

void wakeUpSubThread(struct EventLoop* evLoop)
{
	char* msg = "send msg";
	send(evLoop->socketPair[1], msg, strlen(msg), 0);
	return;
}

int channelDestroy(struct EventLoop* evLoop, struct Channel* channel)
{
	evLoop->channelMap->list[channel->fd] = NULL;
	close(channel->fd);
	free(channel);
	return 0;
}


int RecvMsg(void* arg)
{
	char tmp[24];
	struct EventLoop* evLoop = (struct EventLoop*)arg;
	recv(evLoop->socketPair[0], tmp, sizeof(tmp), 0);
	return 0;
}

struct EventLoop* eventLoopInitEx(char* name)
{
	struct EventLoop* evLoop = (struct EventLoop*)malloc(sizeof(struct EventLoop));
	evLoop->isQuit = false;
	evLoop->dispatcher = &epollDispatcher;
	evLoop->dispatcherData = evLoop->dispatcher->init();
	evLoop->head = evLoop->tail = NULL;
	evLoop->channelMap = ChannelMapInit(128);
	evLoop->threadID = pthread_self();
	strcpy(evLoop->threadName, name == NULL ? "MainThread" : name);	
	pthread_mutex_init(&evLoop->mutex, NULL);
	//添加一对socketPair用于唤醒
	socketpair(AF_UNIX, SOCK_STREAM, 0, evLoop->socketPair);

	struct Channel* channel = ChannelInit(evLoop->socketPair[1], readEvent, RecvMsg, NULL, NULL, evLoop);
	eventLoopAddTask(evLoop, channel, ADD);
	return evLoop;
}

void eventLoopRun(struct EventLoop* evLoop)
{
	assert(NULL != evLoop);

	//启动的线程ID和当前线程ID比较
	if (evLoop->threadID != pthread_self())
	{
		return;
	}

	while (!evLoop->isQuit)
	{
		evLoop->dispatcher->dispatch(evLoop, 2);
		eventLoopProcessTask(evLoop);
	}
}

void eventActive(struct EventLoop* evLoop, int fd, int type)
{
	struct ChannelMap* channelMap = evLoop->channelMap;

	struct Channel* channel = channelMap->list[fd];

	if (fd == channel->fd)
	{
		if (type & readEvent)
		{
			channel->readCallBack(channel->arg);
		}
		else if (type & writeEvent)
		{
			channel->writeCallBack(channel->arg);
		}
	}
}

void eventLoopAddTask(struct EventLoop* evLoop, struct Channel* channel, int type)
{
	pthread_mutex_lock(&evLoop->mutex);
	struct ElementChannel* node = (struct ElementChannel*)malloc(sizeof(struct ElementChannel));
	node->type = type;
	node->channel = channel;
	node->next = NULL;
	if (NULL == evLoop->head)
	{
		evLoop->head = evLoop->tail = node;
	}
	else
	{
		evLoop->tail->next = node;
		evLoop->tail = node;
	}
	pthread_mutex_unlock(&evLoop->mutex);

	//开始处理任务（分主子线程或者说是否是当前线程）
	if (evLoop->threadID == pthread_self())
	{
		eventLoopProcessTask(evLoop);
	}
	else
	{
		//1.子线程正在处理任务 2.唤醒阻塞子线程
		wakeUpSubThread(evLoop);
	}
	
}

void eventLoopProcessTask(struct EventLoop* evLoop)
{
	pthread_mutex_lock(&evLoop->mutex);
	struct ElementChannel* head = evLoop->head;

	while (NULL != head)
	{
		struct ElementChannel* tmp = head;
		if (head->type == ADD)
		{
			eventLoopAdd(evLoop, head->channel);
		}
		else if (head->type == DEL)
		{
			eventLoopDel(evLoop, head->channel);
		}
		else
		{
			eventLoopMod(evLoop, head->channel);
		}
				
		head = head->next;
		free(tmp);
	}

	evLoop->head = evLoop->tail = NULL;
	pthread_mutex_unlock(&evLoop->mutex);
}

void eventLoopAdd(struct EventLoop* evLoop, struct Channel* channel)
{
	int fd = channel->fd;
	int size = evLoop->channelMap->size;
	if (fd >= size)
	{
		if (!ChannelMapMakeUpRoom(evLoop->channelMap, fd, sizeof(struct Channel*)))
		{
			return;
		}
	}

	if (NULL == evLoop->channelMap->list[fd])
	{
		evLoop->channelMap->list[fd] = channel;
		evLoop->dispatcher->add(channel, evLoop);
	}
}

void eventLoopDel(struct EventLoop* evLoop, struct Channel* channel)
{
	int fd = channel->fd;
	int size = evLoop->channelMap->size;
	if (fd >= size)
	{
		exit(0);
	}

	evLoop->dispatcher->remove(channel, evLoop);
}

void eventLoopMod(struct EventLoop* evLoop, struct Channel* channel)
{
	int fd = channel->fd;
	int size = evLoop->channelMap->size;
	if (fd >= size)
	{
		exit(0);
	}

	evLoop->dispatcher->modify(channel, evLoop);
}


