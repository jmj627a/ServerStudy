#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string>

//내 패킹 파일의 기본 헤더
struct PackHeader
{
	//unsigned int code		= 0x0627;		//나만의 코드
	unsigned long FileNum	= 1;			//파일이 몇개나 들어있는가?
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

	//파일 사이즈 읽기
	fseek(pFile, 0, SEEK_END);
	int size = ftell(pFile);
	//printf("%d \n", size);

	//파일 사이즈만큼 할당
	char* pBuffer = (char*)malloc(size+1);
	memset(pBuffer,0, size);

	//파일 포인터 맨 앞으로 돌리기
	rewind(pFile);

	//파일 읽어서 pBuffer에 저장
	int temp = fread(pBuffer, 1, size, pFile);
	pBuffer[size] = '\0';

	printf("%d %s \n", temp, pBuffer);
	  
	free(pBuffer);
	fclose(pFile);
}