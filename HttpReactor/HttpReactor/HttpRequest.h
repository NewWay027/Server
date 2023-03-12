#pragma once

#include <stdbool.h>
#include "HttpResponse.h"

enum HttpReqStatus
{
	HttpReqLine,
	HttpReqHead,
	HttpReqBody,
	HttpReqDone
};

struct HttpRequestHead
{
	char* key;
	char* value;
};

struct HttpRequest
{
	char* method;
	char* url;
	char* version;
	struct HttpRequestHead * httpRequestHeader;
	int httpRequestNum;
	enum HttpReqStatus curStatus;
};

//初始化
struct HttpRequest* HttpRequsetInit();
//重置Http
void HttpRequestReset(struct HttpRequest* request);
//添加请求头
void HttpRequestAddHeader(struct HttpRequest* request, char* key, char* value);
//获取请求头的值
char * HttpRequestGetHeader(struct HttpRequest* request, char* key);  
//解析请求行
bool parseHttpRequestLine(struct HttpRequest* request, struct Buffer * readBuf);
//解析请求头
bool parseHttpRequestHead(struct HttpRequest* request, struct Buffer* readBuf);
//解析Http请求
bool parseHttpRequest(struct HttpRequest* request, struct HttpResponse* response, struct Buffer* readBuf, struct Buffer* writeBuf, int socket);
//处理Http请求
int HttpRequestprocess(struct HttpRequest* request, struct HttpResponse* response);

const char* getFileType(const char* name);
// 发送目录
int sendDir(const char* dirName, struct Buffer* sendBuf, int cfd);
//发送文件
int sendFile(const char* fileName, struct Buffer* sendBuf, int cfd);
int hexToDec(char c);
void decodeMsg(char* to, char* from);
//销毁
void HttpRequestDestroy(struct HttpRequest* request);