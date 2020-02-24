#pragma once

struct NODE
{
	int height;
	NODE* next;
};

class CQueue
{
	NODE* head;
	NODE* tail;

public:
	CQueue()
	{
		head = new NODE();
		tail = new NODE();

		head->height = 0;
		tail->height = 0;
		head->next = tail;
	}

	void enq(int _height);
	bool deq(NODE*& _node);
	void printAll();
};