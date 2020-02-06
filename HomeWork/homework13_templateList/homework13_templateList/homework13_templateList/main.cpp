#include <stdio.h>
#include "CList.h"


void main()
{
	CList<int> list;

	list.push_front(10);
	list.push_front(20);
	list.push_front(30);
	list.push_back(100);
	list.push_back(200);
	list.push_front(40);

	list.printAll();
}