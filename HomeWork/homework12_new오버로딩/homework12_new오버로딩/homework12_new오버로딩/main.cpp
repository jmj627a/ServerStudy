#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>

#include "CList.h"
//#include "CNew.h"

CList* list = new CList();

void* operator new (size_t size, const char* File, int Line);
void* operator new[](size_t size, const char* File, int Line);

void operator delete (void* p, char* File, int Line);
void operator delete[](void* p, char* File, int Line);

void operator delete (void* p);
void operator delete[](void* p);

void* operator new(size_t size, const char* File, int Line)
{
	//printf("!!!! %d !!!!\n", size);
 	void* ptr = malloc(size);

	list->listAdd(ptr, size, Line, File);

	return ptr;
}

void* operator new[](size_t size, const char* File, int Line)
{
	return operator new(size, File, Line);
}

// 다음 delete 는 실제로 쓰진 않지만 문법상 컴파일 오류를 막기 위해 만듬
// 속은 비워둠.

void operator delete (void* p, char* File, int Line)
{
}
void operator delete[](void* p, char* File, int Line)
{
}

// 실제로 사용할 delete
void operator delete (void* p)
{
	list->MemoryRelease(p);
}

void operator delete[](void* p)
{
	operator delete(p);
}


#define __FILENAME__    strrchr(__FILE__, '\\') 
#define new new(__FILENAME__, __LINE__)

void main()
{
	int* a = new int;
	int* p = new int[10];
	char* p1 = new char;
	char* p2 = new char[10];


	delete (int*)0x00331144;
	delete a;
	delete[] p;
	delete p1;
	//delete[] p2;
	
	list->MemoryPrint();
}

