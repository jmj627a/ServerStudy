#pragma warning(disable: 4996)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>

#define _MemoryAlloc(type, num) 	 MemoryAlloc<type>(num,  __LINE__, __FILE__) 

struct AllocInfo
{
	void* ptr;
	int size;
	char FileName[128];
	int FileLine;

	AllocInfo* next;
};

AllocInfo* info;
int a = 1;

template <typename T>
void listAdd(T* temp, int num, int _LineNum, const char* _FileName)
{
	AllocInfo* newNode = (AllocInfo*)malloc(sizeof(AllocInfo));

	//헤드일때 헤드 아닐떄
	if (info == NULL)
	{
		newNode->ptr = temp;
		newNode->size = num * sizeof(T);
		newNode->FileLine = _LineNum;
		strcpy(newNode->FileName, _FileName);
		newNode->next = NULL;

		info = newNode;
		return;
	}
	else
	{
		AllocInfo* t = info;
		while (t->next != NULL)
		{
			t = t->next;
		}

		newNode->ptr = temp;
		newNode->size = num * sizeof(T);
		newNode->FileLine = _LineNum;
		strcpy(newNode->FileName, _FileName);
		newNode->next = NULL;

		t->next = newNode;
		return;
	}
}

template <typename T>
T* MemoryAlloc(int _num, int _LineNum, const char* _FileName)
{
	T* temp = new T[_num];

	listAdd(temp, _num, _LineNum, _FileName);

	return temp;
}

void deleteNode(AllocInfo* Node)
{
	AllocInfo* temp = Node;
	AllocInfo* delNode = Node->next;

	temp->next = temp->next->next;
	delete delNode;
}

template <typename T>
bool MemoryRelease(T* a)
{
	AllocInfo* temp = info;
	T* findPtr = a;

	if (temp->ptr == findPtr)
	{
		info = info->next;
		delete temp;
		return true;
	}

	while (temp->next != NULL)
	{
		if (temp->next->ptr == findPtr)
		{
			deleteNode(temp);
			return true;
		}
		temp = temp->next;
	}

	return false;
}

void MemoryPrint()
{
	AllocInfo* fi = info;
	if (fi == NULL)
		return;

	while (true)
	{
		printf("ptr 주소 : %X \n", (fi->ptr));
		printf("size : %d byte \n", fi->size);
		printf("위치 : %d \n", fi->FileLine);
		printf("이름 : %s \n\n", fi->FileName);


		fi = fi->next;
		if (fi == NULL)
			break;
	}
}


void main()
{

	char* p = _MemoryAlloc(char, 5);
	int* p1 = _MemoryAlloc(int, 10);
	double *p2 = _MemoryAlloc(double, 15);
	double *p3 = _MemoryAlloc(double, 100);
	double *p4 = _MemoryAlloc(double, 100);

	MemoryRelease((int*)0x12341234);
	MemoryRelease(p2);
	MemoryRelease(p1);
	MemoryRelease(p);

	MemoryPrint();
}