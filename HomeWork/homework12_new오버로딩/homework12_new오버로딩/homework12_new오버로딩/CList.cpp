#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>

#include "CList.h"

void CList::listAdd(void* temp, int size, int _LineNum, const char* _FileName)
{
	CNode* newNode = (CNode*)malloc(sizeof(CNode));

	newNode->ptr = temp;
	newNode->size = size;
	newNode->FileLine = _LineNum;
	strcpy(newNode->FileName, _FileName);

	
	CNode* curr = head->next;

	

	newNode->next = curr;
	head->next = newNode;



	return;
}

template <typename T>
T* CList::MemoryAlloc(int _num, int _LineNum, const char* _FileName)
{
	T* temp = new T[_num];

	listAdd(temp, _num, _LineNum, _FileName);

	return temp;
}


bool CList::MemoryRelease(void* a)
{
	CNode* curr = this->head;
	void* findPtr = a;

	if (head->next == tail) return true;

	while (true)
	{
		if (curr->next->ptr == a)
		{
			CNode* temp = curr->next;
			curr->next = curr->next->next;
			//delete temp; 여기서 delete하니까 또 오버로드한 delete가 불림
			return true;
		}

		curr = curr->next;

		if (curr->next == tail)
		{
			//NOALLOC 오류 출력
			return false;
		}
	}
}



void CList::MemoryPrint()
{
	CNode* curr = this->head->next;
	if (curr == tail)
		return;

	while (true)
	{
		//LEAK 오류 출력
		printf("ptr 주소 : %X \n", (curr->ptr));
		printf("size : %d byte \n", curr->size);
		printf("위치 : %d \n", curr->FileLine);
		printf("이름 : %s \n\n", curr->FileName);


		curr = curr->next;
		if (curr == tail)
			break;
	}


}