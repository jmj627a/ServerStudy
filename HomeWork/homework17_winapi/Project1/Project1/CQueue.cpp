#include "CQueue.h"

void  CQueue::enq(int _height)
{
	NODE* newNode = new NODE();
	newNode->height = _height;
	newNode->next = head->next;
	head->next = newNode;
	count++;
}

bool  CQueue::deq(NODE*& _node)
{
	if (head->next == tail)
		return false;

	NODE* curr = head->next;

	while (true)
	{
		//���������� tail�̸� ������ ���� �����̶� tail����
		if (curr->next->next == tail)
		{
			_node = curr->next;
			//delete curr->next;
			curr->next = curr->next->next;
			break;
		}

		curr = curr->next;
	}

	count--;
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

int CQueue::getIndex(int _index)
{
	if (head->next == tail)
		return -10;
	
	int idx = -1;

	NODE* curr = head;
	while (true)
	{
		if (curr == tail)
			return -10;

		if (idx == _index)
			return curr->height;

		idx++;
		curr = curr->next;
	}
}
