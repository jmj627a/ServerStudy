#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>

#include "CList.h"
//#include "CNew.h"

using namespace std;

CList* list = new CList();
FILE *fp;

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
	bool isDeleteSuccess = true;

	if (!list->MemoryRelease(p))
	{
		char buf[22];
		sprintf(buf, "NOALLOC [0x%p]\n", p);
		printf("!!!%s\n",buf);
		fwrite(&buf, 1, 21, fp);
	}
}

void operator delete[](void* p)
{
	operator delete(p);
}


#define __FILENAME__    strrchr(__FILE__, '\\') 
#define new new(__FILENAME__, __LINE__)

void main()
{
	struct tm* today;
	time_t now = time(NULL);
	char file_name[100];

	today = localtime(&now);  // 현재시간
   // sprintf(file_name, "%s", asctime(today)); //현재시간을 file_name에 저장
	sprintf(file_name, "%d%02d%02d_%02d%02d%02d.txt", today->tm_year + 1900, today->tm_mon + 1, today->tm_mday
	,today->tm_hour, today->tm_min, today->tm_sec);


	fp = fopen(file_name, "w+");

	int* a = new int;
	int* p = new int[10];
	char* p1 = new char;
	char* p2 = new char[10];


	delete (int*)0x00331144;
	//delete a;
	//delete[] p;
	delete p1;
	//delete[] p2;
	
	//list->MemoryPrint();

	CNode* curr = list->head->next;
	if (curr == list->tail)
		return;

	while (true)
	{
		//LEAK 오류 출력
		char buf[100];
		sprintf(buf, "LEAK [0x%p] size[%d] [%s] \t : %d \n", 
			curr->ptr, curr->size, curr->FileName, curr->FileLine);
		fprintf(fp, buf);

		curr = curr->next;
		if (curr == list->tail)		break;
	}

	fclose(fp);
}

