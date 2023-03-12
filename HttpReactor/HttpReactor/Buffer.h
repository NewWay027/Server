#pragma once
#include <stdbool.h>

struct Buffer
{
	char *data;
	int readPos;
	int writePos;
	int capacity;
};

//初始化
struct Buffer* BufferInit(int size);

//获取可读空间
int bufferReadableSize(struct Buffer * buffer);
//获取可写空间
int bufferWritableSize(struct Buffer* buffer);
//添加数据两种方式1.指定size；2.不指定size
int bufferAppendData(struct Buffer* buffer, const char * data, int size);
int bufferAppendString(struct Buffer* buffer, const char* data);
//与套接字通信
int bufferSocketData(struct Buffer * buffer, int fd);
//获取换行位置
char* getBufferCRLF(char * data);
//发送数据
int bufferSendData(struct Buffer* buffer, int fd);
//扩容
bool bufferMakeUpRoom(struct Buffer* buffer, int size);
//销毁
void bufferDestroy(struct Buffer* buffer);

