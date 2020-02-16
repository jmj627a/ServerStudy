#include "CMessageQueue.h"

bool CMessageQueue::Enq(int message)
{
	NODE* newNode = new NODE();
	newNode->message_type = message;
	newNode->prev = head;
	head->next->prev = newNode;
	newNode->next = head->next;
	head->next = newNode;
	return true;
}

bool CMessageQueue::Deq(int* message)
{
	if (head->next == tail)
		return false;

	NODE* temp = head->next;
	*message = temp->message_type;
	head->next = head->next->next;
	delete temp;
	temp = nullptr;
	return true;
}
