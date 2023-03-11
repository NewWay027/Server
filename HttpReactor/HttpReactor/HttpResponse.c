#include "HttpResponse.h"
#include "Buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RESPNSEHEADNUM 12
struct HttpResponse* HttpResponseInit()
{
    struct HttpResponse* response = (struct HttpResponse*)malloc(sizeof(struct HttpResponse));
    response->curStatusCode = Unkouwn;
	memset(response->fileName, 0, sizeof(response->fileName));
	memset(response->statusMsg, 0, sizeof(response->statusMsg));
    response->httpResponseHeader = (struct HttpResponseHeader*)malloc(RESPNSEHEADNUM * sizeof(struct HttpResponseHeader));
	memset(response->httpResponseHeader, 0, RESPNSEHEADNUM * sizeof(struct HttpResponseHeader));
    response->httpResponseHeaderNum = 0; 
    return response;
}


void HttpResponseAddHeader(struct HttpResponse* response, char* key, char* value)
{
	strncpy(response->httpResponseHeader[response->httpResponseHeaderNum].key, key, strlen(key));
	strncpy(response->httpResponseHeader[response->httpResponseHeaderNum].value, value, strlen(value));
	response->httpResponseHeaderNum++;
}

char* HttpResponseGetHeader(struct HttpResponse* response, char* key)
{
	for (int i = 0; i < response->httpResponseHeader; i++)
	{
		if (strcasecmp(response->httpResponseHeader[i].key, key) == 0)
		{
			return response->httpResponseHeader[i].value;
		}
	}
	return NULL;
}

void HttpResponsePrepareMsg(struct HttpResponse* response, struct Buffer* sendBuf, int socket)
{
	char resLine[128] = { 0 };
	//响应行
	sprintf(resLine, "HTTP/1.1 %d %s\r\n", response-> curStatusCode, response->statusMsg);
	bufferAppendString(sendBuf, resLine);
	//响应头
	char tmpBuf[4096] = { 0 };
	for (int i = 0; i < response->httpResponseHeaderNum; i++)
	{
		sprintf(tmpBuf, "%s: %s\r\n", response->httpResponseHeader[i].key
			, response->httpResponseHeader[i].value);
		bufferAppendString(sendBuf, tmpBuf);
	}
	//空行
	bufferAppendString(sendBuf, "\r\n");
	bufferSendData(sendBuf, socket);
	//响应数据
	response->senDataFunc(response->fileName, sendBuf, socket);
	return NULL;
}

void HttpResponseDestroy(struct HttpResponse* response)
{
	free(response->httpResponseHeader);
	free(response);
}

