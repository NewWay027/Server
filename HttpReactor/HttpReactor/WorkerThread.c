#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "WorkerThread.h"

void WorkerThreadInit(struct WorkerThread* thread, int index)
{
	thread->threadID = 0;
	sscanf(thread->threadName, "SubWorkerThread-%s", index);
	thread->evLoop = NULL;
	pthread_mutex_init(&thread->mutex, NULL);
	pthread_cond_init(&thread->cond, NULL);
}

void* subThreadRunn(void* arg)
{
	struct WorkerThread* thread = (struct WorkerThread*)arg;
	pthread_mutex_lock(&thread->mutex);
	thread->evLoop = eventLoopInitEx(NULL);
	pthread_mutex_unlock(&thread->mutex);
	pthread_cond_signal(&thread->cond);
	eventLoopRun(thread->evLoop);
	return NULL;
}

void WorkerThreadRun(struct WorkerThread* thread)
{
	pthread_create(&thread->threadID, NULL, subThreadRunn, thread);
	pthread_mutex_lock(&thread->mutex);
	while (NULL == thread->evLoop)
	{
		pthread_cond_wait(&thread->cond, &thread->mutex);
	}
	pthread_mutex_unlock(&thread->mutex);
}


