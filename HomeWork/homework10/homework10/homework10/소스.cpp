#pragma warning(disable: 4996)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define _MemoryAlloc(type, num) 	 MemoryAlloc<type>(num,  __LINE__, __FILE__) 

struct AllocInfo
{
	void* ptr;
	int size;
	char FileName[128];
	int FileLine;

	AllocInfo* next;
};

AllocInfo* info = (AllocInfo*)malloc(sizeof(AllocInfo));

template <typename T>
void listAdd(int* temp, int num, int _LineNum, const char* _FileName)
{
	if (info == NULL)
	{
		info->ptr = temp;
		info->size = num;
		info->FileLine = _LineNum;
		strcpy(info->FileName, _FileName);
	}
}


template <typename T>
T* MemoryAlloc(int _num, int _LineNum, const char* _FileName)
{
	T* temp = new T[_num];


	listAdd<T>(temp, _num, _LineNum, _FileName);


	return temp;
}

template <typename T>
void MemoryRelease(T* a)
{

}

void MemoryPrint()
{
	AllocInfo* fi = info;
	while (fi->next != NULL)
	{
		printf("ptr 주소 : %d \n", fi->ptr);
		printf("size : %d \n", fi->size);
		printf("위치 : %d \n", fi->FileLine);
		printf("이름 : %s \n", fi->FileName);


		fi = fi->next;
	}
}


void main()
{
	int* p = _MemoryAlloc(int, 10);


	MemoryPrint();
}