#pragma once

#include <stdbool.h>
#include <pthread.h>
#include "Channel.h"
#include "Dispatcher.h"
#include "ChannelMap.h"

struct Dispatcher;

extern struct Dispatcher epollDispatcher;

enum ElementType
{
	ADD,
	DEL,
	MOD
};

struct ElementChannel {
	int type;
	struct Channel* channel;
	struct ElementChannel* next;
};

struct EventLoop
{
	bool isQuit;
	struct Dispatcher * dispatcher;
	void* dispatcherData;

	//����
	struct ElementChannel* head;
	struct ElementChannel* tail;

	//map
	struct ChannelMap* channelMap;

	//�߳�
	pthread_t threadID;
	char threadName[128];
	pthread_mutex_t mutex;
	
	//socketPair
	int socketPair[2];
};

//��ʼ��EventLoop
struct EventLoop* eventLoopInit();
struct EventLoop* eventLoopInitEx(char * name);
//������Ӧ��
void eventLoopRun(struct EventLoop* evLoop);
//�¼�����
void eventActive(struct EventLoop* evLoop, int fd, int type);
//�������
void eventLoopAddTask(struct EventLoop* evLoop, struct Channel * channel, int type);
//��������
void eventLoopProcessTask(struct EventLoop* evLoop);
//��Ӧ�����channel
void eventLoopAdd(struct EventLoop* evLoop, struct Channel * channel);
//��Ӧ��ɾ��channel
void eventLoopDel(struct EventLoop* evLoop, struct Channel * channel);
//��Ӧ���޸�channel
void eventLoopMod(struct EventLoop* evLoop, struct Channel* channel);
//�����߳�
void wakeUpSubThread(struct EventLoop* evLoop);
//�ͷ�
int channelDestroy(struct EventLoop* evLoop, struct Channel* channel);