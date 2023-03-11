#pragma once
#include "Buffer.h"
// ����һ������ָ��, ������֯Ҫ�ظ����ͻ��˵����ݿ�
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

//��ʼ����Ӧ
struct HttpResponse* HttpResponseInit();
//��ӻ�ȡ����ͷ
void HttpResponseAddHeader(struct HttpResponse* response, char* key, char* value);
char* HttpResponseGetHeader(struct HttpResponse* response, char* key);
//׼�����͵�����
void HttpResponsePrepareMsg(struct HttpResponse* response, struct Buffer * sendBuf, int socket);
//����
void HttpResponseDestroy(struct HttpResponse* response);