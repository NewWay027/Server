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

bool ChannelMapMakeUpRoom(struct ChannelMap* channelMap, int fd)
{
	if (fd < channelMap->size)
	{
		return true;
	}

	struct Channel** tmpMap = (struct Channel**)realloc(channelMap->list, channelMap->size + fd);
	if (NULL == tmpMap)
	{
		return false;
	}

	//将原来的拷贝到重新分配的内存空间
	memcpy(tmpMap, channelMap->list, channelMap->size);
	
	channelMap->list = tmpMap;
	channelMap->size = channelMap->size + fd;
	
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
		free(channelMap);
		channelMap = NULL;
	}
}
