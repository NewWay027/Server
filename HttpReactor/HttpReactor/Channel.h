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

//��ʼ��Channel
struct Channel* ChannelInit(int fd, int events, CallBaclFunc readCallBack, CallBaclFunc writeCallBack, CallBaclFunc destroyCallBack, void* arg);
//ʹ��д�¼�
int writeEventEnable(struct Channel* channel);
//��ȡд�¼�ʹ��
bool isWriteEventEnable(struct Channel* channel);
//�ͷ���Դ
void ChannelDestroy(struct Channel* channel);