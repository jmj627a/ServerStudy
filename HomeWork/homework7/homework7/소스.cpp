#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string>

//�� ��ŷ ������ �⺻ ���
struct PackHeader
{
	//unsigned int code		= 0x0627;		//������ �ڵ�
	unsigned long FileNum	= 1;			//������ ��� ����ִ°�?
};

struct PackFile
{
	char FileName[64];
	unsigned long size;
	unsigned long offset;
};

void main()
{
	FILE* pFile = fopen("1.txt", "rb");

	//fputs("This is an apple.", pFile);
	//fputs(" sam", pFile);

	//���� ������ �б�
	fseek(pFile, 0, SEEK_END);
	int size = ftell(pFile);
	//printf("%d \n", size);

	//���� �����ŭ �Ҵ�
	char* pBuffer = (char*)malloc(size+1);
	memset(pBuffer,0, size);

	//���� ������ �� ������ ������
	rewind(pFile);

	//���� �о pBuffer�� ����
	int temp = fread(pBuffer, 1, size, pFile);
	pBuffer[size] = '\0';

	printf("%d %s \n", temp, pBuffer);
	  
	free(pBuffer);
	fclose(pFile);
}