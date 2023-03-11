#pragma once
#include <pthread.h>
#include "EventLoop.h"
#include "WorkerThread.h"

struct ThreadPool {
	bool isStart;
	int threadNum;
	struct WorkerThread* workerThread;
	int index;
	struct EventLoop* mainLoop;
};

//初始化一个线程池
struct ThreadPool* ThreadPoolInit(struct EventLoop* mainLoop, int threadNum);
//启动线程池
void ThreadPoolRun(struct ThreadPool* pool);
//从线程池中取出一个反应堆
struct EventLoop* takeOneEventLoop(struct ThreadPool* pool);

