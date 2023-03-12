#include "Buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/uio.h>
#include <unistd.h>

struct Buffer* BufferInit(int size)
{
	struct Buffer* buffer = (struct Buffer*)malloc(sizeof(struct Buffer));
	buffer->data = (char*)malloc(size);
	buffer->readPos = 0;
	buffer->writePos = 0;
	buffer->capacity = size;
	memset(buffer->data, 0, size);
	return buffer;
}

int bufferReadableSize(struct Buffer* buffer)
{
	return buffer->writePos - buffer->readPos;
}

int bufferWritableSize(struct Buffer* buffer)
{
	return buffer->capacity - buffer->writePos;
}

int bufferAppendData(struct Buffer* buffer, const char* data, int size)
{
	if (!bufferMakeUpRoom(buffer, size))
	{
		exit(0);
	}

	memcpy(buffer->data + buffer->writePos, data, size);
	buffer->writePos += size;
	
	return 0;
}

int bufferAppendString(struct Buffer* buffer, const char* data)
{
	int size = strlen(data);
	bufferAppendData(buffer, data, size);
	return 0;
}

int bufferSocketData(struct Buffer* buffer, int fd)
{
	int writable = buffer->capacity - buffer->writePos;
	struct iovec iov[2];
	iov[0].iov_base = buffer->data + buffer->writePos;
	iov[0].iov_len = writable;

	char* tmpBuf = (char*)malloc(40960);
	iov[1].iov_base = tmpBuf;
	iov[1].iov_len = 40960;

	int len = readv(fd, iov, 2);
	if (-1 == len)
	{
		return -1;
	}
	else if (len <= writable)
	{
		buffer->writePos += len;
	}
	else
	{
		buffer->writePos = buffer->capacity;
		bufferAppendData(buffer, tmpBuf, len - writable);
	}
	free(tmpBuf);
	return len;
}

char* getBufferCRLF(char* data)
{
	char * ptr = memmem(data, strlen(data), "\r\n", 2);
	return ptr;
}

int bufferSendData(struct Buffer* buffer, int fd)
{
	int readable = bufferReadableSize(buffer);
	if (readable > 0)
	{
		int count = send(fd, buffer->data + buffer->readPos, readable, 0);
		if (count > 0)
		{
			buffer->readPos += count;
			usleep(1);
		}
		return count;
	}
	return 0;
}

bool bufferMakeUpRoom(struct Buffer* buffer, int size)
{
	int writable = bufferWritableSize(buffer);
	int readable = bufferReadableSize(buffer);
	//1.可写  >= size
	if (writable >= size)
	{
		return true;
	}
	//2.已读+未写>=size
	else if (buffer->readPos + writable >= size)
	{
		//1.先移动数据
		memcpy(buffer->data, buffer->data + buffer->readPos, readable);
		buffer->readPos = 0;
		buffer->writePos = readable;
		return true;
	}
	//3.扩容
	{
		int curCapacity = buffer->capacity;
		while (curCapacity - buffer->writePos <= size)
		{
			curCapacity *= 2;
		}

		char* tmpbuf = (char *)realloc(buffer->data, curCapacity);
		if (NULL == tmpbuf)
		{
			exit(0);
		}
	
		memset(buffer->data + buffer->capacity, 0, curCapacity - buffer->capacity);

		buffer->data = tmpbuf;
		buffer->capacity = curCapacity;
		return true;
	}

	return false;
}

void bufferDestroy(struct Buffer* buffer)
{
	if (NULL != buffer)
	{
		if (NULL != buffer->data)
		{
			free(buffer->data);
		}
	}
	free(buffer);
}
