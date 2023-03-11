#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "HttpRequest.h"
#include "Buffer.h"

#define HEADNUM 12

struct HttpRequest* HttpRequsetInit()
{
	struct HttpRequest* request = (struct HttpRequest*)malloc(sizeof(struct HttpRequest));
	request->url = NULL;
	request->method = NULL;
	request->version = NULL;
	request->httpRequestHeader = (struct HttpRequestHead*)malloc(HEADNUM * sizeof(struct HttpRequestHead));
	memset(request->httpRequestHeader, 0, HEADNUM * sizeof(struct HttpRequestHead));
	request->curStatus = HttpReqLine;
	request->httpRequestNum = 0;
	return request;
}

void HttpRequestAddHeader(struct HttpRequest* request, char* key, char* value)
{
	request->httpRequestHeader[request->httpRequestNum].key = key;
	request->httpRequestHeader[request->httpRequestNum].value = value;
	request->httpRequestNum++;
}

char* HttpRequestGetHeader(struct HttpRequest* request, char* key)
{
	for (int i = 0; i < request->httpRequestNum; i++)
	{
		if (strcasecmp(request->httpRequestHeader[i].key, key) == 0)
		{
			return request->httpRequestHeader[i].value;
		}
	}
	return NULL;
}

bool parseHttpRequestLine(struct HttpRequest* request, struct Buffer* readBuf)
{
	char* end = getBufferCRLF(readBuf->data + readBuf->readPos);
	char* start = readBuf->data + readBuf->readPos;
	int lineSapce = end - start;

	char * ptr = memmem(start, end - start, " ", 1);
	int len = ptr - start;
	request->method = (char *)malloc(len + 1);
	strncpy(request->method, start, len);
	request->method[len] = '\0';

	start = ptr + 1;
	ptr = memmem(start, end - start, " ", 1);
	len = ptr - start;
	request->url = (char*)malloc(len + 1);
	strncpy(request->url, start, len);
	request->url[len] = '\0';

	start = ptr + 1;
	len = end - start;
	request->version = (char*)malloc(len + 1);
	strncpy(request->version, start, len);
	request->version[len] = '\0';

	readBuf->readPos = readBuf->readPos + lineSapce + 2;
	request->curStatus = HttpReqHead;
	return true;
}

bool parseHttpRequestHead(struct HttpRequest* request, struct Buffer* readBuf)
{
	char* end = getBufferCRLF(readBuf->data + readBuf->readPos);
	if (NULL != end)
	{
		char* start = readBuf->data + readBuf->readPos;
		int lineSapce = end - start;

		char* middle = memmem(start, end - start, ": ", 2);
		if (NULL != middle)
		{
			char* key = (char*)malloc(middle - start + 1);
			strncpy(key, start, middle - start);
            start = middle + 2;

			char* value = (char*)malloc(end - start + 1);
			strncpy(value, start, end - start);

            HttpRequestAddHeader(request, key, value);
			readBuf->readPos = readBuf->readPos + lineSapce + 2;
		}
		else
		{
			readBuf->readPos += 2;
			request->curStatus = HttpReqDone;
		}
	}
	else
	{
		return false;
	}
	return true;
}

bool parseHttpRequest(struct HttpRequest* request, struct HttpResponse* response, struct Buffer* readBuf, struct Buffer* writeBuf, int socket)
{
	bool flag = true;
	while (request->curStatus != HttpReqDone)
	{
		switch (request->curStatus)
		{
		case HttpReqLine:
			flag = parseHttpRequestLine(request, readBuf);
			break;
		case HttpReqHead:
			flag = parseHttpRequestHead(request, readBuf);
			break;
		}
	}

	//解析完就要开始处理
    HttpRequestprocess(request, response, readBuf, writeBuf, socket);
    //准备响应数据
    HttpResponsePrepareMsg(response, writeBuf, socket);
	return flag;
}

int HttpRequestprocess(struct HttpRequest* request, struct HttpResponse* response, struct Buffer* readBuf, struct Buffer* writeBuf, int cfd)
{
	if (0 != strcasecmp("get", request->method))
	{
		return -1;
	}

	char* file = NULL;
	if (0 == strcmp(request->url, "/"))
	{
		file = "./";
	}
	else
	{
		file = request->url + 1;
	}
    //decodeMsg(file, file);
	struct stat st;
	
	int ret = stat(file, &st);
	if (-1 == ret)
	{
		//文件资源不存在404
		response->curStatusCode = NotFound;
		strcpy(response->fileName, "404.html");
		strcpy(response->statusMsg, "Not Found");
		HttpResponseAddHeader(response, "Content-Type", getFileType("404.html"));
        response->senDataFunc = sendFile;
		return;
	}

	response->curStatusCode = OK;
	strcpy(response->fileName, file);
	strcpy(response->statusMsg, "OK");
	if (S_ISDIR(st.st_mode))
	{
		//发送目录
		HttpResponseAddHeader(response, "Content-Type", getFileType(".html"));
        response->senDataFunc = sendDir;
	}
	else
	{
		//发送文件
        HttpResponseAddHeader(response, "Content-Type", getFileType(file));
        char size[24] = {0};
        sprintf(size, "%ld", st.st_size);
        HttpResponseAddHeader(response, "Content-Length", size);
        response->senDataFunc = sendFile;
	}
	
	return 0;
}

const char* getFileType(const char* name)
{
    // a.jpg a.mp4 a.html
    // 自右向左查找‘.’字符, 如不存在返回NULL
    const char* dot = strrchr(name, '.');
    if (dot == NULL)
        return "text/plain; charset=utf-8";	// 纯文本
    if (strcmp(dot, ".html") == 0 || strcmp(dot, ".htm") == 0)
        return "text/html; charset=utf-8";
    if (strcmp(dot, ".jpg") == 0 || strcmp(dot, ".jpeg") == 0)
        return "image/jpeg";
    if (strcmp(dot, ".gif") == 0)
        return "image/gif";
    if (strcmp(dot, ".png") == 0)
        return "image/png";
    if (strcmp(dot, ".css") == 0)
        return "text/css";
    if (strcmp(dot, ".au") == 0)
        return "audio/basic";
    if (strcmp(dot, ".wav") == 0)
        return "audio/wav";
    if (strcmp(dot, ".avi") == 0)
        return "video/x-msvideo";
    if (strcmp(dot, ".mov") == 0 || strcmp(dot, ".qt") == 0)
        return "video/quicktime";
    if (strcmp(dot, ".mpeg") == 0 || strcmp(dot, ".mpe") == 0)
        return "video/mpeg";
    if (strcmp(dot, ".vrml") == 0 || strcmp(dot, ".wrl") == 0)
        return "model/vrml";
    if (strcmp(dot, ".midi") == 0 || strcmp(dot, ".mid") == 0)
        return "audio/midi";
    if (strcmp(dot, ".mp3") == 0)
        return "audio/mpeg";
    if (strcmp(dot, ".ogg") == 0)
        return "application/ogg";
    if (strcmp(dot, ".pac") == 0)
        return "application/x-ns-proxy-autoconfig";

    return "text/plain; charset=utf-8";
}

/*
<html>
    <head>
        <title>test</title>
    </head>
    <body>
        <table>
            <tr>
                <td></td>
                <td></td>
            </tr>
            <tr>
                <td></td>
                <td></td>
            </tr>
        </table>
    </body>
</html>
*/
int sendDir(const char* dirName, struct Buffer *sendBuf, int cfd)
{
    char buf[4096] = { 0 };
    sprintf(buf, "<html><head><title>%s</title></head><body><table>", dirName);
    bufferAppendString(sendBuf, buf);
    struct dirent** namelist;
    int num = scandir(dirName, &namelist, NULL, alphasort);
    for (int i = 0; i < num; ++i)
    {
        // 取出文件名 namelist 指向的是一个指针数组 struct dirent* tmp[]
        char* name = namelist[i]->d_name;
        struct stat st;
        char subPath[1024] = { 0 };
        sprintf(subPath, "%s/%s", dirName, name);
        stat(subPath, &st);
        if (S_ISDIR(st.st_mode))
        {
            // a标签 <a href="">name</a>
            sprintf(buf + strlen(buf),
                "<tr><td><a href=\"%s/\">%s</a></td><td>%ld</td></tr>",
                name, name, st.st_size);
        }
        else
        {
            sprintf(buf + strlen(buf),
                "<tr><td><a href=\"%s\">%s</a></td><td>%ld</td></tr>",
                name, name, st.st_size);
        }
        //send(cfd, buf, strlen(buf), 0);
        bufferAppendString(sendBuf, buf);
        memset(buf, 0, sizeof(buf));
        free(namelist[i]);
    }
    sprintf(buf, "</table></body></html>");
    //send(cfd, buf, strlen(buf), 0);
    bufferAppendString(sendBuf, buf);
    bufferSendData(sendBuf, cfd);
    free(namelist);
    return 0;
}


int sendFile(const char* fileName, struct Buffer* sendBuf, int cfd)
{
    // 1. 打开文件
    int fd = open(fileName, O_RDONLY);
    assert(fd > 0);
#if 1 
    while (1)
    {
        char buf[1024] = {0};
        int len = read(fd, buf, sizeof buf);
        if (len > 0)
        {
            bufferAppendData(sendBuf, buf, len);
            bufferSendData(sendBuf, cfd);
            //send(cfd, buf, len, 0);
            //usleep(10); // 这非常重要
        }
        else if (len == 0)
        {
            break;
        }
        else
        {
            perror("read");
        }
    }
#else
    off_t offset = 0;
    int size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    while (offset < size)
    {
        int ret = sendfile(cfd, fd, &offset, size - offset);
        printf("ret value: %d\n", ret);
        if (ret == -1 && errno == EAGAIN)
        {
            printf("没数据...\n");
        }
    }
#endif
    close(fd);
    return 0;
}

// 将字符转换为整形数
int hexToDec(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;

    return 0;
}

// 解码
// to 存储解码之后的数据, 传出参数, from被解码的数据, 传入参数
void decodeMsg(char* to, char* from)
{
    for (; *from != '\0'; ++to, ++from)
    {
        // isxdigit -> 判断字符是不是16进制格式, 取值在 0-f
        // Linux%E5%86%85%E6%A0%B8.jpg
        if (from[0] == '%' && isxdigit(from[1]) && isxdigit(from[2]))
        {
            // 将16进制的数 -> 十进制 将这个数值赋值给了字符 int -> char
            // B2 == 178
            // 将3个字符, 变成了一个字符, 这个字符就是原始数据
            *to = hexToDec(from[1]) * 16 + hexToDec(from[2]);

            // 跳过 from[1] 和 from[2] 因此在当前循环中已经处理过了
            from += 2;
        }
        else
        {
            // 字符拷贝, 赋值
            *to = *from;
        }

    }
    *to = '\0';
}

void HttpRequestDestroy(struct HttpRequest* request)
{
    if (NULL == request)
    {
        return;
    }

    free(request->method);
    free(request->url);
    free(request->version);
    for (int i = 0; i < request->httpRequestNum; i++)
    {
        free(request->httpRequestHeader[i].key);
        free(request->httpRequestHeader[i].value);
    }

    free(request);
}

