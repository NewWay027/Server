#pragma once
#include <stdbool.h>
#include "Channel.h"

struct ChannelMap
{
	int size;
	struct Channel** list;
};

//��ʼ��ChannelMap
struct ChannelMap* ChannelMapInit(int size);
//�ռ䲻��ʱ��Ҫ����
bool ChannelMapMakeUpRoom(struct ChannelMap* channelMap, int size, int unitSize);
//����ChannelMap
void destroyChannelMap(struct ChannelMap* channelMap);
