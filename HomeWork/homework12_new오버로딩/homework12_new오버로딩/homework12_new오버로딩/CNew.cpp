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

// ���� delete �� ������ ���� ������ ������ ������ ������ ���� ���� ����
// ���� �����.

void CNew:: operator delete (void* p, char* File, int Line)
{
}
void CNew::operator delete[](void* p, char* File, int Line)
{
}

// ������ ����� delete
void CNew::operator delete (void* p)
{
}
void CNew::operator delete[](void* p)
{
}