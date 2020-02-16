#pragma once
#include "global.h"

struct NODE
{
	int message_type;
	NODE* next;
	NODE* prev;
};


class CMessageQueue
{
public:
	NODE* head;
	NODE* tail;

	bool Enq(int message);
	bool Deq(int* message);

	CMessageQueue()
	{
		head = new NODE();
		tail = new NODE();

		head->message_type = DUMMY;
		head->next = tail;

		tail->message_type = DUMMY;
		tail->prev = head;
	};

	~CMessageQueue()
	{
		NODE* curr = head;
		while (curr != tail)
		{
			NODE* temp = curr;
			curr = curr->next;

			delete temp;
		}

		delete tail;
	};
};

