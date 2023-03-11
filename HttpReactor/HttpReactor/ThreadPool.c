#include <stdio.h>
#include <stdlib.h>
#include "ThreadPool.h"

struct ThreadPool* ThreadPoolInit(struct EventLoop* mainLoop, int threadNum)
{
	struct ThreadPool* pool = (struct ThreadPool*)malloc(sizeof(struct ThreadPool));
	pool->index = 0;
	pool->isStart = false;
	pool->mainLoop = mainLoop;
	pool->threadNum = threadNum;
	pool->workerThread = (struct WorkerThread*)calloc(threadNum, sizeof(struct WorkerThread));
	return pool;
}

void ThreadPoolRun(struct ThreadPool* pool)
{
	if (pool->isStart)
	{
		return;
	}

	if (pool->threadNum > 0)
	{
		for (int i = 0; i < pool->threadNum; ++i)
		{
			WorkerThreadInit(&pool->workerThread[i], i);
			WorkerThreadRun(&pool->workerThread[i]);
		}
		
	}
}

struct EventLoop* takeOneEventLoop(struct ThreadPool* pool)
{
	struct EventLoop* evLoop = pool->mainLoop;
	if (pool->threadNum > 0)
	{
		evLoop = pool->workerThread[pool->index].evLoop;
		pool->index = ++(pool->index) % pool->threadNum;
	}
	return evLoop;
}
