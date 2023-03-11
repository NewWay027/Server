#include "Dispatcher.h"
#include <sys/epoll.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define EpollNum 1024
struct EpollDispatcherData
{
	int epfd;
	struct epoll_event *events;
};

static void* epollInit();
static void epollAdd(struct Channel* channel, struct EventLoop* evLoop);
static void epollRemove(struct Channel* channel, struct EventLoop* evLoop);
static void epollModify(struct Channel* channel, struct EventLoop* evLoop);
static void epollDispatch(struct EventLoop* evLoop, int timeout);
static void epollDestroy(struct EventLoop* evLoop);
static int epollCtl(struct Channel* channel, struct EventLoop* evLoop, int type);

struct Dispatcher epollDispatcher =
{
	epollInit,
	epollAdd,
	epollRemove,
	epollModify,
	epollDispatch,
	epollDestroy
};

void *epollInit()
{
	struct EpollDispatcherData * epollData = (struct EpollDispatcherData *)malloc(sizeof(struct EpollDispatcherData));
	epollData->epfd = epoll_create(128);
	if (-1 == epollData->epfd)
	{
		perror("epoll_create");
	}
	epollData->events = (struct epoll_event*)calloc(EpollNum, sizeof(struct epoll_event));
	return epollData;
}

void epollAdd(struct Channel* channel, struct EventLoop* evLoop)
{
	int ret = epollCtl(channel, evLoop, EPOLL_CTL_ADD);
	if (-1 == ret)
	{
		perror("epoll_ctl");
	}
}

void epollRemove(struct Channel* channel, struct EventLoop* evLoop)
{
	int ret = epollCtl(channel, evLoop, EPOLL_CTL_DEL);
	if (-1 == ret)
	{
		perror("epoll_ctl");
	}
}

void epollModify(struct Channel* channel, struct EventLoop* evLoop)
{
	int ret = epollCtl(channel, evLoop, EPOLL_CTL_MOD);
	if (-1 == ret)
	{
		perror("epoll_ctl");
	}
}

void epollDispatch(struct EventLoop* evLoop, int timeout)
{
	struct EpollDispatcherData* epollData = (struct EpollDispatcherData*)evLoop->dispatcherData;

	int num = epoll_wait(epollData->epfd, epollData->events, 1024, timeout * 1000);

	if (-1 == num)
	{
		perror("epoll_wait");
	}

	for (int i = 0; i < num; i++)
	{
		int fd = epollData->events[i].data.fd;
		if (epollData->events[i].events == EPOLLIN)
		{
			eventActive(evLoop, fd, readEvent);
		}
		if (epollData->events[i].events == EPOLLOUT)
		{
			eventActive(evLoop, fd, writeEvent);
		}
	}
}

void epollDestroy(struct EventLoop* evLoop)
{
	struct EpollDispatcherData* epollData = (struct EpollDispatcherData*)evLoop->dispatcherData;
	free(epollData->events);
	close(epollData->epfd);
	free(epollData);
}

int epollCtl(struct Channel* channel, struct EventLoop* evLoop, int type)
{
	struct EpollDispatcherData* epollData = (struct EpollDispatcherData*)evLoop->dispatcherData;
	
	int ret;
	struct epoll_event ev;
	
	if (channel->events == readEvent)
	{
		ev.data.fd = channel->fd;
		ev.events = EPOLLIN;
		ret = epoll_ctl(epollData->epfd, type, channel->fd, &ev);
	}
	if (channel->events == writeEvent)
	{
		ev.data.fd = channel->fd;
		ev.events = EPOLLOUT;
		ret = epoll_ctl(epollData->epfd, type, channel->fd, &ev);
	}

	return ret;
}
