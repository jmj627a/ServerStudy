/*
 -전역변수 -

int g_Data = 0;
int g_Connect = 0;
bool g_Shutdown = false;


g_Connect 는 접속자 수를 가상으로 표현.
g_Data 는 가상의 데이터 처리를 표현.


# AcceptThread

100 ~ 1000ms 마다 랜덤하게 g_Connect 를 + 1

# DisconnectThread

100 ~ 1000ms 마다 랜덤하게 g_Connect 를 - 1


# UpdateThread x 3

10ms 마다 g_Data 를 + 1

그리고 g_Data 가 1000 단위가 될 때 마다 이를 출력


# main

1초마다 g_Connect 를 출력
20초 후 g_Shutdown = true 로 다른 스레드들 종료.

스레드 종료 확인 후 main 종료.



+ beginthreadex 를 사용

+ 모든 변수 및 출력은 동기화가 이루어지도록 안전하게 되어야 함.
+ interlocked 로 해결되는 경우는 interlocked 사용.
+ interlocked 로 해결이 안되는 경우는 critical_section 사용.
+ critical_section 사용 후 srw 으로 변경 테스트
+ 모든 스레드는 할일이 없는경우 block 상태로 쉬도록 합니다.

+ 모든 스레드는 g_Shutdown 이 true 가 되면 종료됨.

+ main 스레드는 모든 스레드가 종료 된 후에 종료처리.

+ 메인에서 스레드의 종료 확인은 WaitForMultipleObjects 사용.

*/

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <process.h>

LONG g_Data = 0;
LONG g_Connect = 0;
LONG g_Shutdown = false;

CRITICAL_SECTION cs;

//100 ~1000ms 마다 랜덤하게 g_Connect 를 + 1
unsigned __stdcall AcceptThread(void* arg)
{
	while (true)
	{
		EnterCriticalSection(&cs);
		if (g_Shutdown)
		{
			LeaveCriticalSection(&cs);
			return true;
		}
		else
			LeaveCriticalSection(&cs);
		
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
		EnterCriticalSection(&cs);
		if (g_Shutdown)
		{
			LeaveCriticalSection(&cs);
			return true;
		}
		else
			LeaveCriticalSection(&cs);

		Sleep(rand() % 900 + 100);
		int ret= InterlockedDecrement(&g_Connect);
		//wprintf(L"%ld\n", g_Connect);
	}
}

//10ms 마다 g_Data 를 + 1
//그리고 g_Data 가 1000 단위가 될 때 마다 이를 출력
unsigned __stdcall UpdateThread(void* arg)
{
	while (true)
	{
		EnterCriticalSection(&cs);
		if (g_Shutdown)
		{
			LeaveCriticalSection(&cs);
			return true;
		}
		else
			LeaveCriticalSection(&cs);

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
	DWORD dwThreadID;
	HANDLE hThread[5];
	InitializeCriticalSection(&cs);

	hThread[0] = (HANDLE)_beginthreadex(NULL, 0, AcceptThread, NULL, 0, (unsigned int*)&dwThreadID);
	hThread[1] = (HANDLE)_beginthreadex(NULL, 0, DisconnectThread, NULL, 0, (unsigned int*)&dwThreadID);

	hThread[2] = (HANDLE)_beginthreadex(NULL, 0, UpdateThread, NULL, 0, (unsigned int*)&dwThreadID);
	hThread[3] = (HANDLE)_beginthreadex(NULL, 0, UpdateThread, NULL, 0, (unsigned int*)&dwThreadID);
	hThread[4] = (HANDLE)_beginthreadex(NULL, 0, UpdateThread, NULL, 0, (unsigned int*)&dwThreadID);

	int count = 0;

	while (count < 20)
	{
		Sleep(1000);

		EnterCriticalSection(&cs);
		wprintf(L"count %d : %ld \n", count, g_Connect);
		LeaveCriticalSection(&cs);
	
		count++;
	}

	//여기서만 write
	//g_Shutdown = true;
	InterlockedExchange(&g_Shutdown, true);

	WaitForMultipleObjects(5, hThread, FALSE, INFINITE);

	DeleteCriticalSection(&cs);
	system("pause");

}