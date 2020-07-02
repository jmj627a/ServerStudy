/*
 -�������� -

int g_Data = 0;
int g_Connect = 0;
bool g_Shutdown = false;


g_Connect �� ������ ���� �������� ǥ��.
g_Data �� ������ ������ ó���� ǥ��.


# AcceptThread

100 ~ 1000ms ���� �����ϰ� g_Connect �� + 1

# DisconnectThread

100 ~ 1000ms ���� �����ϰ� g_Connect �� - 1


# UpdateThread x 3

10ms ���� g_Data �� + 1

�׸��� g_Data �� 1000 ������ �� �� ���� �̸� ���


# main

1�ʸ��� g_Connect �� ���
20�� �� g_Shutdown = true �� �ٸ� ������� ����.

������ ���� Ȯ�� �� main ����.



+ beginthreadex �� ���

+ ��� ���� �� ����� ����ȭ�� �̷�������� �����ϰ� �Ǿ�� ��.
+ interlocked �� �ذ�Ǵ� ���� interlocked ���.
+ interlocked �� �ذ��� �ȵǴ� ���� critical_section ���.
+ critical_section ��� �� srw ���� ���� �׽�Ʈ
+ ��� ������� ������ ���°�� block ���·� ������ �մϴ�.

+ ��� ������� g_Shutdown �� true �� �Ǹ� �����.

+ main ������� ��� �����尡 ���� �� �Ŀ� ����ó��.

+ ���ο��� �������� ���� Ȯ���� WaitForMultipleObjects ���.

*/

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <process.h>

LONG g_Data = 0;
LONG g_Connect = 0;
LONG g_Shutdown = false;

CRITICAL_SECTION cs;

//100 ~1000ms ���� �����ϰ� g_Connect �� + 1
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

//100 ~1000ms ���� �����ϰ� g_Connect �� - 1
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

//10ms ���� g_Data �� + 1
//�׸��� g_Data �� 1000 ������ �� �� ���� �̸� ���
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

	//���⼭�� write
	//g_Shutdown = true;
	InterlockedExchange(&g_Shutdown, true);

	WaitForMultipleObjects(5, hThread, FALSE, INFINITE);

	DeleteCriticalSection(&cs);
	system("pause");

}