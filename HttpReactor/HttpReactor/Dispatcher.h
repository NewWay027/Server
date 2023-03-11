#pragma once
#include "EventLoop.h"
#include "Channel.h"

struct EventLoop;

struct Dispatcher
{
	void* (*init)();
	void (*add)(struct Channel* channel, struct EventLoop* evLoop);
	void (*remove)(struct Channel* channel, struct EventLoop* evLoop);
	void (*modify)(struct Channel* channel, struct EventLoop* evLoop);
	void (*dispatch)(struct EventLoop* evLoop, int timeout);
	void (*destroy)(struct EventLoop* evLoop);
};