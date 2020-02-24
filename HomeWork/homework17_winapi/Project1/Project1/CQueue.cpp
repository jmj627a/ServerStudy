#include "CQueue.h"

void  CQueue::enq(int _height)
{
	NODE* newNode = new NODE();
	newNode->height = _height;
	newNode->next = head->next;
	head->next = newNode;
}

bool  CQueue::deq(NODE*& _node)
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


void CQueue::printAll()
{
	if (head->next == tail)
		return;

	NODE* curr = head->next;

	while (true)
	{
		if (curr == tail)
			return;

		//printf("%d \n", curr->height);

		curr = curr->next;
	}
}