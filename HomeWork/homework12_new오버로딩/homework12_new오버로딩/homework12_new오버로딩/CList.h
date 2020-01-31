#pragma once

class CNode
{
public:
	void* ptr;
	int size;
	char FileName[128];
	int FileLine;

	CNode* next;

	CNode()
	{
		ptr = nullptr;
		size = 0;
		FileName[0] = '\0';
		FileLine = 0;

		next = NULL;
	}
};


class CList
{
public:
	CNode* head;
	CNode* tail;

	CList()
	{
		head = new CNode();
		tail = new CNode();
		head->next = tail;
		tail->next = NULL;
	}


	void listAdd(void* temp, int num, int _LineNum, const char* _FileName);

	template <typename T>
	T* MemoryAlloc(int _num, int _LineNum, const char* _FileName);

	bool MemoryRelease(void* a);

	void MemoryPrint();
};

