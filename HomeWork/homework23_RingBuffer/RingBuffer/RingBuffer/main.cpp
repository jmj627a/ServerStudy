#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>
#include <stdlib.h>

#include "RingBuf.h"


RingBuf buffer;

char data[] = { "I can hear you but I won't. Some look for trouble while others don't. There's a thousand reasons I should go about my da" };

void main()
{
	srand(10);

	int random;

	char* ptr  = data;
	//char deqCopy[sizeof(data)];
	char* deqCopy = (char*)malloc(sizeof(data));
	int leftSize = sizeof(data);

	while (true)
	{
		if (leftSize == 0)
		{
			ptr = data;
			leftSize = sizeof(data);
		}

		random = rand() % leftSize;
		int ret = buffer.Enqueue(ptr, random); //랜덤한 길이만큼 넣기 
		ptr += ret; //enq된 사이즈만큼 증가.
		random = rand() % leftSize;
		leftSize -= ret;

		ZeroMemory(deqCopy, sizeof(data));
		buffer.Dequeue(deqCopy, random);


		printf("%s", deqCopy);
		Sleep(10);
	}

}