#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>

#include "CNew.h"
#include "CList.h"

void* CNew:: operator new (size_t size, const char* File, int Line)
{
	void* ptr = malloc(size);
	
	//CList::listAdd(ptr, size, Line, File, info);

	return ptr;
}

void* CNew::operator new[](size_t size, const char* File, int Line)
{
	void* ptr = malloc(size);

	//CList::listAdd(ptr, size, Line, File, info);

	return ptr;
}

// 다음 delete 는 실제로 쓰진 않지만 문법상 컴파일 오류를 막기 위해 만듬
// 속은 비워둠.

void CNew:: operator delete (void* p, char* File, int Line)
{
}
void CNew::operator delete[](void* p, char* File, int Line)
{
}

// 실제로 사용할 delete
void CNew::operator delete (void* p)
{
}
void CNew::operator delete[](void* p)
{
}