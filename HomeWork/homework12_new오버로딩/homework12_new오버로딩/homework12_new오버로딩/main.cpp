#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>

#include "CList.h"
//#include "CNew.h"

CList* info = new CList();

void* operator new (size_t size, const char* File, int Line);
void* operator new[](size_t size, const char* File, int Line);

void operator delete (void* p, char* File, int Line);
void operator delete[](void* p, char* File, int Line);

void operator delete (void* p);
void operator delete[](void* p);

void* operator new(size_t size, const char* File, int Line)
{
	printf("!!!! %d !!!!\n", size);
 	void* ptr = malloc(size);

	info->listAdd(ptr, size, Line, File);

	return ptr;
}

void* operator new[](size_t size, const char* File, int Line)
{
	return operator new(size, File, Line);
}

// ���� delete �� ������ ���� ������ ������ ������ ������ ���� ���� ����
// ���� �����.

void operator delete (void* p, char* File, int Line)
{
}
void operator delete[](void* p, char* File, int Line)
{
}

// ������ ����� delete
void operator delete (void* p)
{
}
void operator delete[](void* p)
{
}


#define __FILENAME__    strrchr(__FILE__, '\\') 
#define new new(__FILENAME__, __LINE__)


void main()
{
	int* a = new int;
	int* p = new int[10];
	char* p1 = new char;
	char* p2 = new char[10];

	//MemoryRelease((int*)0x12341234);
	//MemoryRelease(p2);
	//MemoryRelease(p1);
	//MemoryRelease(p);
	//
	//MemoryPrint();
}

