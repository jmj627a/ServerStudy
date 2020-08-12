#include <Windows.h>
#include <process.h>
#include <iostream>
#include "LockFreeStack.h"

#define dfMAX_THREAD	4
#define	dfMAX_DATA		1000
#define dfMAX_All_DATA	(dfMAX_THREAD * dfMAX_DATA)

#pragma comment(lib,"winmm")

struct st_DATA {

	LONG64 iData = 0x00000000ffffffff;
	LONG64 iCount = 0;

};

LockFreeStack<st_DATA*> stack;

unsigned int __stdcall Thread(void* arg) {

	st_DATA* pData[dfMAX_DATA];

	printf("BeginThread\n");

	for (;;) {
		for (int i = 0; i < dfMAX_DATA; i++) {

			stack.Pop(&pData[i]);

			//printf("[%d] POP\n",pData[i]);
		}

		for (int i = 0; i < dfMAX_DATA; i++) {

			if (pData[i]->iData != 0x00000000ffffffff || pData[i]->iCount != 0) {
				int a = 0;
			}

			InterlockedIncrement64(&pData[i]->iData);
			InterlockedIncrement64(&pData[i]->iCount);
		}

		for (int i = 0; i < dfMAX_DATA; i++) {

			if (pData[i]->iData != 0x0000000100000000 || pData[i]->iCount != 1) {
				int a = 0;
			}
			InterlockedDecrement64(&pData[i]->iData);
			InterlockedDecrement64(&pData[i]->iCount);
		}

		for (int i = 0; i < dfMAX_DATA; i++) {

			//printf("[%d] PUSH\n", pData[i]);

			stack.Push(pData[i]);
		}

		//printf("StackSize : %d KeyCount : %lld\n", LFStack.GetStackSize(), LFStack.GetKeyCount());
	}
	return 0;
}

int main() {

	timeBeginPeriod(1);

	unsigned int uiID;
	HANDLE hThread[dfMAX_THREAD];

	for (int i = 0; i < dfMAX_All_DATA; i++) {

		st_DATA* pData = new st_DATA;

		stack.Push(pData);
	}

	for (int i = 0; i < dfMAX_THREAD; i++) {

		hThread[i] = (HANDLE)_beginthreadex(NULL, 0, Thread, NULL, 1, &uiID);
	}

	for (;;) {

		if (GetAsyncKeyState(VK_SHIFT) & 0001) {

			printf("%d\n", stack.GetStackSize());
		}
		printf("%d\n", stack.GetStackSize());
		Sleep(1000);
	}

	timeEndPeriod(1);


	return 0;
}