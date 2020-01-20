#pragma warning(disable: 4996)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>

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

	//헤드일때 헤드 아닐떄
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
		AllocInfo* t = info;
		while (t->next != NULL)
		{
			t = t->next;
		}

		newNode->ptr = temp;
		newNode->size = num * sizeof(int);
		newNode->FileLine = _LineNum;
		strcpy(newNode->FileName, _FileName);
		newNode->next = NULL;

		t->next = newNode;
		return;
	}
}


int* MemoryAlloc(int _num, int _LineNum, const char* _FileName)
{
	int* temp = new int[_num];

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

bool MemoryRelease(int* a)
{
	AllocInfo* temp = info;
	int* findPtr = a;

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

	int* p = _MemoryAlloc(int, 5);
	int* p1 = _MemoryAlloc(int, 10);
	int* p2 = _MemoryAlloc(int, 15);

	MemoryRelease((int*)0x12341234);
	MemoryRelease(p2);
	//MemoryRelease(p1);
	MemoryRelease(p);

	MemoryPrint();
}