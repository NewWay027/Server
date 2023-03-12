#pragma once
#include <stdbool.h>

struct Buffer
{
	char *data;
	int readPos;
	int writePos;
	int capacity;
};

//��ʼ��
struct Buffer* BufferInit(int size);

//��ȡ�ɶ��ռ�
int bufferReadableSize(struct Buffer * buffer);
//��ȡ��д�ռ�
int bufferWritableSize(struct Buffer* buffer);
//����������ַ�ʽ1.ָ��size��2.��ָ��size
int bufferAppendData(struct Buffer* buffer, const char * data, int size);
int bufferAppendString(struct Buffer* buffer, const char* data);
//���׽���ͨ��
int bufferSocketData(struct Buffer * buffer, int fd);
//��ȡ����λ��
char* getBufferCRLF(char * data);
//��������
int bufferSendData(struct Buffer* buffer, int fd);
//����
bool bufferMakeUpRoom(struct Buffer* buffer, int size);
//����
void bufferDestroy(struct Buffer* buffer);

