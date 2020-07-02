#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <process.h>

LONG g_Data = 0;
LONG g_Connect = 0;
LONG g_Shutdown = false;

SRWLOCK g_srwLock;

//100 ~1000ms 마다 랜덤하게 g_Connect 를 + 1
unsigned __stdcall AcceptThread(void* arg)
{
	while (true)
	{
		AcquireSRWLockShared(&g_srwLock);
		if (g_Shutdown)
		{
			ReleaseSRWLockShared(&g_srwLock);
			return true;
		}
		else
			ReleaseSRWLockShared(&g_srwLock);

		Sleep(rand() % 900 + 100);
		int ret = InterlockedIncrement(&g_Connect);
		//wprintf(L"%ld\n", g_Connect);
	}
}

//100 ~1000ms 마다 랜덤하게 g_Connect 를 - 1
unsigned __stdcall DisconnectThread(void* arg)
{
	while (true)
	{
		AcquireSRWLockShared(&g_srwLock);
		if (g_Shutdown)
		{
			ReleaseSRWLockShared(&g_srwLock);
			return true;
		}
		else
			ReleaseSRWLockShared(&g_srwLock);

		Sleep(rand() % 900 + 100);
		int ret = InterlockedDecrement(&g_Connect);
		//wprintf(L"%ld\n", g_Connect);
	}
}

//10ms 마다 g_Data 를 + 1
//그리고 g_Data 가 1000 단위가 될 때 마다 이를 출력
unsigned __stdcall UpdateThread(void* arg)
{
	while (true)
	{
		AcquireSRWLockShared(&g_srwLock);
		if (g_Shutdown)
		{
			ReleaseSRWLockShared(&g_srwLock);
			return true;
		}
		else
			ReleaseSRWLockShared(&g_srwLock);

		Sleep(10);
		int retData = InterlockedIncrement(&g_Data);

		if (retData % 1000 == 0)
		{
			wprintf(L"%d : %d \n", GetCurrentThreadId(), retData);
		}
	}
}

void main()
{
	int count = 0;
	DWORD dwThreadID;
	HANDLE hThread[5];

	InitializeSRWLock(&g_srwLock);

	hThread[0] = (HANDLE)_beginthreadex(NULL, 0, AcceptThread, NULL, 0, (unsigned int*)&dwThreadID);
	hThread[1] = (HANDLE)_beginthreadex(NULL, 0, DisconnectThread, NULL, 0, (unsigned int*)&dwThreadID);
	hThread[2] = (HANDLE)_beginthreadex(NULL, 0, UpdateThread, NULL, 0, (unsigned int*)&dwThreadID);
	hThread[3] = (HANDLE)_beginthreadex(NULL, 0, UpdateThread, NULL, 0, (unsigned int*)&dwThreadID);
	hThread[4] = (HANDLE)_beginthreadex(NULL, 0, UpdateThread, NULL, 0, (unsigned int*)&dwThreadID);

	while (count < 20)
	{
		Sleep(1000);
		AcquireSRWLockShared(&g_srwLock);
		wprintf(L"count %d : %ld \n", count, g_Connect);
		ReleaseSRWLockShared(&g_srwLock);
		count++;
	}

	//여기서만 write
	//g_Shutdown = true;
	AcquireSRWLockExclusive(&g_srwLock);
	g_Shutdown = true;
	ReleaseSRWLockExclusive(&g_srwLock);

	WaitForMultipleObjects(5, hThread, FALSE, INFINITE);

	system("pause");

}