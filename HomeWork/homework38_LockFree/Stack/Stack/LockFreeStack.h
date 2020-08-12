#pragma once

#include <Windows.h>
#include <list>

using namespace std;

template<class DATA>
class LockFreeStack
{
private:
	//Node�� ���� (���� ���*, ������)
	struct st_NODE {
		st_NODE* pNext = NULL;
		DATA	data;
	};

private:
	st_NODE*	pTop;

public:
	LockFreeStack() { pTop = new st_NODE; }
	~LockFreeStack() { }

	//1. newNode ����
	//2. newNode->next = top ����
	//3. if(top ���� �˴� top?)
	//4. true -> top = newNode
	//5. false -> 2~4 �ݺ�
	void Push(DATA data) {

		st_NODE* tempNode;

		//1.
		st_NODE* pNewNode = new st_NODE;
		pNewNode->data = data;

		do {
			//2.
			pNewNode->pNext = pTop;
			//�̷��� 2step���� �ϴ� ���� -> �׳� top �����Ϳ� top�� newNode->next�� ���ϸ� 
			//tempNode = pTop;
			//newNode->pNext = tempNode;

			//3.
		} while (!InterlockedCompareExchange128((LONG64*)&pTop, (LONG64)pTop, (LONG64)pNewNode, (LONG64*)&pNewNode->pNext));

		InterlockedIncrement(&_lStackSize);
	}

	bool Pop(INT* pData) {

		st_NODE cmpNode;
		do {

			cmpNode = _top;
			//pDel = cmpNode.pNode;
			//cmpNode.pNode->pNext;

		} while (!InterlockedCompareExchange128(
			(LONG64*)&_top,
			(LONG64)cmpNode.pNext->pNext,
			(LONG64)cmpNode.pNext,
			(LONG64*)&cmpNode));

		InterlockedDecrement(&_lStackSize);
		*pData = cmpNode.pNode->data;
		delete cmpNode.pNode;

		return true;
	}

	long	GetStackSize() { return _lStackSize; }
	LONG64	GetKeyCount() { return _l64KeyCount; }
};