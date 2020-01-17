#pragma warning(disable: 4996)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define _MemoryAlloc(type, num) 	 MemoryAlloc(num,  __LINE__, __FILE__) 

struct AllocInfo
{
	void* ptr;
	int size;
	char FileName[128];
	int FileLine;

	AllocInfo* next;
};

AllocInfo* info;

void listAdd(int* temp, int num, int _LineNum, const char* _FileName)
{
	AllocInfo* newNode = (AllocInfo*)malloc(sizeof(AllocInfo));

	if (info == NULL)
	{
		newNode->ptr = temp;
		newNode->size = num * sizeof(int);
		newNode->FileLine = _LineNum;
		strcpy(newNode->FileName, _FileName);
		newNode->next = NULL;

		info = newNode;
		return;
	}
	else
	{
		while (info->next != NULL)
		{
			info = info->next;
		}

		newNode->ptr = temp;
		newNode->size = num * sizeof(int);
		newNode->FileLine = _LineNum;
		strcpy(newNode->FileName, _FileName);
		newNode->next = NULL;

		info->next = newNode;
		return;
	}
}


int* MemoryAlloc(int _num, int _LineNum, const char* _FileName)
{
	int* temp = new int[_num];


	listAdd(temp, _num, _LineNum, _FileName);


	return temp;
}

void MemoryRelease(int* a)
{

}

void MemoryPrint()
{
	AllocInfo* fi = info;
	while (true)
	{
		printf("ptr 주소 : %d \n", fi->ptr);
		printf("size : %d byte \n", fi->size);
		printf("위치 : %d \n", fi->FileLine);
		printf("이름 : %s \n", fi->FileName);


		fi = fi->next;
		if (fi == NULL)
			break;
	}
}


void main()
{

	int* p = _MemoryAlloc(int, 10);
	int* p1 = _MemoryAlloc(int, 5);


	MemoryPrint();
}