#include "ChannelMap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

struct ChannelMap* ChannelMapInit(int size)
{
	struct ChannelMap* channelMap = (struct ChannelMap*)malloc(sizeof(struct ChannelMap));
	channelMap->size = size;
	channelMap->list = (struct Channel**)calloc(size, sizeof(struct Channel*));
	return channelMap;
}

bool ChannelMapMakeUpRoom(struct ChannelMap* channelMap, int size, int unitSize)
{
	if (size < channelMap->size)
	{
		return true;
	}
	int curSize= channelMap->size;
	while (curSize < size)
	{
		curSize *= 2;
	}

	struct Channel** tmpMap = (struct Channel**)realloc(channelMap->list, unitSize * curSize);
	if (NULL == tmpMap)
	{
		return false;
	}

	//直接指定新的地址即可，不需要去移动数据
	channelMap->list = tmpMap;
	memset(channelMap->list[channelMap->size] , 0, curSize - channelMap->size);
	channelMap->size = curSize;
	
	return true;
}

void destroyChannelMap(struct ChannelMap* channelMap)
{
	if (NULL != channelMap)
	{
		for (int i = 0; i < channelMap->size; i++)
		{
			free(channelMap->list[i]);
		}
		free(channelMap->list);
		channelMap->list = NULL;
	}
	channelMap->size = 0;
}
