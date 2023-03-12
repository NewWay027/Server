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

	//队列
	struct ElementChannel* head;
	struct ElementChannel* tail;

	//map
	struct ChannelMap* channelMap;

	//线程
	pthread_t threadID;
	char threadName[128];
	pthread_mutex_t mutex;
	
	//socketPair
	int socketPair[2];
};

//初始化EventLoop
struct EventLoop* eventLoopInit();
struct EventLoop* eventLoopInitEx(char * name);
//启动反应堆
void eventLoopRun(struct EventLoop* evLoop);
//事件激活
void eventActive(struct EventLoop* evLoop, int fd, int type);
//添加任务
void eventLoopAddTask(struct EventLoop* evLoop, struct Channel * channel, int type);
//处理任务
void eventLoopProcessTask(struct EventLoop* evLoop);
//反应堆添加channel
void eventLoopAdd(struct EventLoop* evLoop, struct Channel * channel);
//反应堆删除channel
void eventLoopDel(struct EventLoop* evLoop, struct Channel * channel);
//反应堆修改channel
void eventLoopMod(struct EventLoop* evLoop, struct Channel* channel);
//唤醒线程
void wakeUpSubThread(struct EventLoop* evLoop);
//释放
int channelDestroy(struct EventLoop* evLoop, struct Channel* channel);