#pragma once
#include <pthread.h>
#include "EventLoop.h"

struct WorkerThread
{
	pthread_t threadID;
	char threadName[32];
	struct EventLoop * evLoop;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
};

//��ʼ�������߳�
void WorkerThreadInit(struct WorkerThread* thread, int index);
void WorkerThreadRun(struct WorkerThread* thread);