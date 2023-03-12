#pragma once
#include <stdbool.h>

typedef int (*CallBaclFunc)(void* arg);

enum ChannelEvent
{
	readEvent = 0x02,
	writeEvent = 0x04
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
void writeEventEnable(struct Channel* channel, bool isWrite);
//��ȡд�¼�ʹ��
bool isWriteEventEnable(struct Channel* channel);
//�ͷ���Դ
void ChannelDestroy(struct Channel* channel);