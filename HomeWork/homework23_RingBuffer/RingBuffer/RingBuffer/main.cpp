#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>


#include "RingBuf.h"


RingBuf buffer;

char data[] = { "I can hear you but I won't. Some look for trouble while others don't. There's a thousand reasons I should go about my da" };

void main()
{
	srand(30);

	int enqRandom;
	int deqRandom;

	char* ptr  = data;
	//char deqCopy[sizeof(data)];
	char* deqCopy = (char*)malloc(sizeof(data));
	int leftSize = sizeof(data)-1;

	while (true)
	{
		//printf("%s", data);

		if (leftSize == 0)
		{
			ptr = data;
			leftSize = sizeof(data)-1;
		}
		
		enqRandom = rand() % leftSize + 1;
		int enqRet = buffer.Enqueue(ptr, enqRandom); //랜덤한 길이만큼 넣기 
		ptr += enqRet; //enq된 사이즈만큼 증가.
		
		deqRandom = rand() % leftSize + 1;

		ZeroMemory(deqCopy, sizeof(data));
		int peekRet = buffer.Peek(deqCopy, deqRandom);

		leftSize -= enqRet;
		
		int deqRet = buffer.Dequeue(deqCopy, deqRandom);

		if (deqRet > 0)
			printf("%s", deqCopy);



		Sleep(10);
	}

}