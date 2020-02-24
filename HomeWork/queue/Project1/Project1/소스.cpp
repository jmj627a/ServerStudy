#pragma once
#include <stdio.h>

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

	void enq(int _height)
	{
		NODE* newNode = new NODE();
		newNode->height = _height;
		newNode->next = head->next;
		head->next = newNode;
	}

	bool deq(NODE*& _node)
	{
		if (head->next == tail)
			return false;

		NODE* curr = head->next;

		while (true)
		{
			//다음다음이 tail이면 다음거 빼고 지금이랑 tail연결
			if (curr->next->next == tail)
			{
				_node = curr->next;
				//delete curr->next;
				curr->next = curr->next->next;
				break;
			}

			curr = curr->next;
		}

		return true;
	}


	void printAll()
	{
		if (head->next == tail)
			return;

		NODE* curr = head->next;

		while (true)
		{
			if (curr == tail)
				return;

			printf("%d \n", curr->height);

			curr = curr->next;
		}
	}

};

CQueue queue;
void main()
{
	queue.enq(10);
	queue.enq(200);
	queue.enq(30);
	queue.enq(40);
	queue.enq(5);
	queue.enq(600);

	NODE* temp = new NODE();
	queue.deq(temp);

	queue.printAll();

}