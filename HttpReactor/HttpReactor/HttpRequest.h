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

//��ʼ��
struct HttpRequest* HttpRequsetInit();
//����Http
void HttpRequestReset(struct HttpRequest* request);
//�������ͷ
void HttpRequestAddHeader(struct HttpRequest* request, char* key, char* value);
//��ȡ����ͷ��ֵ
char * HttpRequestGetHeader(struct HttpRequest* request, char* key);  
//����������
bool parseHttpRequestLine(struct HttpRequest* request, struct Buffer * readBuf);
//��������ͷ
bool parseHttpRequestHead(struct HttpRequest* request, struct Buffer* readBuf);
//����Http����
bool parseHttpRequest(struct HttpRequest* request, struct HttpResponse* response, struct Buffer* readBuf, struct Buffer* writeBuf, int socket);
//����Http����
int HttpRequestprocess(struct HttpRequest* request, struct HttpResponse* response);

const char* getFileType(const char* name);
// ����Ŀ¼
int sendDir(const char* dirName, struct Buffer* sendBuf, int cfd);
//�����ļ�
int sendFile(const char* fileName, struct Buffer* sendBuf, int cfd);
int hexToDec(char c);
void decodeMsg(char* to, char* from);
//����
void HttpRequestDestroy(struct HttpRequest* request);