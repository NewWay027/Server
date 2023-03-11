#pragma once
#include "Buffer.h"
// 定义一个函数指针, 用来组织要回复给客户端的数据块
typedef void (*responseBody)(const char* fileName, struct Buffer* sendBuf, int socket);

enum HttpResStatusCode
{
	Unkouwn = 0,
	OK = 200,
	NotFound = 404
};

struct HttpResponseHeader
{
	char key[32];
	char value[128];
};

struct HttpResponse
{
	enum HttpResStatusCode curStatusCode;
	char statusMsg[128];
	struct HttpResponseHeader* httpResponseHeader;
	int httpResponseHeaderNum;
	char fileName[128];
	responseBody senDataFunc;
};

//初始化响应
struct HttpResponse* HttpResponseInit();
//添加获取请求头
void HttpResponseAddHeader(struct HttpResponse* response, char* key, char* value);
char* HttpResponseGetHeader(struct HttpResponse* response, char* key);
//准备发送的数据
void HttpResponsePrepareMsg(struct HttpResponse* response, struct Buffer * sendBuf, int socket);
//销毁
void HttpResponseDestroy(struct HttpResponse* response);