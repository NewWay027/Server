#pragma once
#include <stdbool.h>

typedef int (*CallBaclFunc)(void* arg);

enum ChannelEvent
{
	readEvent = 0,
	writeEvent = 1
};

struct Channel
{
	int fd;
	int events;
	CallBaclFunc readCallBack;
	CallBaclFunc writeCallBack;
	CallBaclFunc destroyCallBack;
	void* arg;
};

//初始化Channel
struct Channel* ChannelInit(int fd, int events, CallBaclFunc readCallBack, CallBaclFunc writeCallBack, CallBaclFunc destroyCallBack, void* arg);
//使能写事件
int writeEventEnable(struct Channel* channel);
//获取写事件使能
bool isWriteEventEnable(struct Channel* channel);
//释放资源
void ChannelDestroy(struct Channel* channel);