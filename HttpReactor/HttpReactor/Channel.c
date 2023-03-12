#include "Channel.h"
#include <stdlib.h>
#include <unistd.h>

struct Channel* ChannelInit(int fd, int events, CallBaclFunc readCallBack, CallBaclFunc writeCallBack, CallBaclFunc destroyCallBack, void* arg)
{
	struct Channel* channel = (struct Channel*)malloc(sizeof(struct Channel));
	channel->fd = fd;
	channel->events = events;
	channel->readCallBack = readCallBack;
	channel->writeCallBack = writeCallBack;
	channel->destroyCallBack = destroyCallBack;
	channel->arg = arg;
	return channel;
}

void writeEventEnable(struct Channel* channel, bool isWrite)
{
	if (isWrite)
	{
		channel->events |= writeEvent;
	}
	else
	{
		channel->events = channel->events & ~writeEvent;
	}	
}

bool isWriteEventEnable(struct Channel* channel)
{
	return channel->events & writeEvent;
}

void ChannelDestroy(struct Channel* channel)
{
	close(channel->fd);
	free(channel);
}
