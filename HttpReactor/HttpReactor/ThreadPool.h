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

//��ʼ��һ���̳߳�
struct ThreadPool* ThreadPoolInit(struct EventLoop* mainLoop, int threadNum);
//�����̳߳�
void ThreadPoolRun(struct ThreadPool* pool);
//���̳߳���ȡ��һ����Ӧ��
struct EventLoop* takeOneEventLoop(struct ThreadPool* pool);

