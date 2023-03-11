#pragma once
#include <stdbool.h>
#include "Channel.h"

struct ChannelMap
{
	int size;
	struct Channel** list;
};

//初始化ChannelMap
struct ChannelMap* ChannelMapInit(int size);
//空间不够时需要扩容
bool ChannelMapMakeUpRoom(struct ChannelMap* channelMap, int fd);
//销毁ChannelMap
void destroyChannelMap(struct ChannelMap* channelMap);
