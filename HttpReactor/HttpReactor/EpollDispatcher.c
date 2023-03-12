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
		exit(0);
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
		exit(0);
	}
}

void epollRemove(struct Channel* channel, struct EventLoop* evLoop)
{
	int ret = epollCtl(channel, evLoop, EPOLL_CTL_DEL);
	if (-1 == ret)
	{
		perror("epoll_ctl");
		exit(0);
	}
	channel->destroyCallBack(channel->arg);
}

void epollModify(struct Channel* channel, struct EventLoop* evLoop)
{
	int ret = epollCtl(channel, evLoop, EPOLL_CTL_MOD);
	if (-1 == ret)
	{
		perror("epoll_ctl");
		exit(0);
	}
}

void epollDispatch(struct EventLoop* evLoop, int timeout)
{
	struct EpollDispatcherData* epollData = (struct EpollDispatcherData*)evLoop->dispatcherData;

	int num = epoll_wait(epollData->epfd, epollData->events, EpollNum, timeout * 1000);

	if (-1 == num)
	{
		perror("epoll_wait");
		exit(0);
	}

	for (int i = 0; i < num; i++)
	{
		int fd = epollData->events[i].data.fd;
		int event = epollData->events[i].events;
		if (event & EPOLLERR || event & EPOLLHUP)
		{
			continue;
		}
		if (event & EPOLLIN)
		{
			eventActive(evLoop, fd, readEvent);
		}
		if (event & EPOLLOUT)
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
	
	struct epoll_event ev;
	ev.data.fd = channel->fd;
	if (channel->events & readEvent)
	{	
		ev.events |= EPOLLIN;
	}
	if (channel->events & writeEvent)
	{
		ev.events |= EPOLLOUT;
	}

	int ret = epoll_ctl(epollData->epfd, type, channel->fd, &ev);
	return ret;
}
