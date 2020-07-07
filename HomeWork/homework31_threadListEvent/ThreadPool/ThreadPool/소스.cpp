/*
DATA : STL List < ���� >


MainThread

		Ư�� Űī ������ SaveThread �� �����.
		Ư�� Ű�� ������ ��� �����带 ���� ��Ŵ.
		��� �����尡 ���� �Ǿ����� Ȯ�� �� ����.


PrintThread


		1�ʸ��� List �� �����͸� ���

		 1-2-59-20-39-9-192-85-2-385-282-2-3- ....


DeleteThread

		  333ms ���� List �� �� �����͸� ����


WorkerThread x 3

		  1�ʸ��� List �� rand �� ����.


SaveThread

		  ���ν������� �̺�Ʈ�� ���� �����
		  ����� List �� ������ ���� TXT �� �����Ѵ�.
		  (2ȸ �̻� ����� ���� ������ ����� �����Ѵ�)

		  ���� : Sleep �� ���� ����.
*/

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <process.h>
#include <list>

//�̺�Ʈ - save �̺�Ʈ - auto�����̺�Ʈ
HANDLE g_Save_autoResetEvent = false;

//�̺�Ʈ - ���� �̺�Ʈ - manual�����̺�Ʈ
HANDLE g_Shutdown_ManualResetEvent = false;

//list
std::list<int> g_dataList;

//list �� SRWLock
SRWLOCK datalist_srw;

//1�ʸ��� List �� �����͸� ���
//1 - 2 - 59 - 20 - 39 - 9 - 192 - 85 - 2 - 385 - 282 - 2 - 3 - ....
unsigned __stdcall PrintThread(void* arg)
{
	int ret;
	std::list<int>::iterator iter;
	while (true)
	{
		ret = WaitForSingleObject(g_Shutdown_ManualResetEvent, 1000);
		if (ret == WAIT_OBJECT_0)
			return 0;

		AcquireSRWLockShared(&datalist_srw);
		for (iter = g_dataList.begin(); iter != g_dataList.end(); ++iter)
			wprintf(L"%d ", (*iter));
		wprintf(L"\n");
		ReleaseSRWLockShared(&datalist_srw);
	}
}

//333ms ���� List �� �� �����͸� ����
unsigned __stdcall DeleteThread(void* arg)
{
	int ret;
	while (true)
	{
		ret = WaitForSingleObject(g_Shutdown_ManualResetEvent, 333);
		if (ret == WAIT_OBJECT_0)
			return 0;

		AcquireSRWLockExclusive(&datalist_srw);
		if(g_dataList.size() != 0)
			g_dataList.pop_back();
		ReleaseSRWLockExclusive(&datalist_srw);
	}
}

//1�ʸ��� List �� rand �� ����.
unsigned __stdcall WorkerThread(void* arg)
{
	int ret;
	srand(GetCurrentThreadId());
	while (true)
	{
		ret = WaitForSingleObject(g_Shutdown_ManualResetEvent, 1000);
		if (ret == WAIT_OBJECT_0)
			return 0;

		AcquireSRWLockExclusive(&datalist_srw);
		g_dataList.push_back(rand() % 10000);
		ReleaseSRWLockExclusive(&datalist_srw);
	}
}

//���ν������� �̺�Ʈ�� ���� �����
//����� List �� ������ ���� TXT �� �����Ѵ�.
//(2ȸ �̻� ����� ���� ������ ����� �����Ѵ�)
unsigned __stdcall SaveThread(void* arg)
{
	FILE *fp = NULL;
	std::list<int>::iterator iter;

	while (1) {
		int ret;
		
		ret = WaitForSingleObject(g_Shutdown_ManualResetEvent, 1);
		if (ret == WAIT_OBJECT_0)
			return 0;

		ret = WaitForSingleObject(g_Save_autoResetEvent, 1);
		if (ret == WAIT_OBJECT_0)
		{
			fopen_s(&fp, "List.txt", "w");
			
			if (NULL == fp)
				printf("fopen_s Fail\n");

			AcquireSRWLockShared(&datalist_srw);
			for (iter = g_dataList.begin(); iter != g_dataList.end(); ++iter)
				fprintf_s(fp, "%d ", (*iter));
			ReleaseSRWLockShared(&datalist_srw);

			fclose(fp);
			ResetEvent(g_Save_autoResetEvent);
		}
	}
}

// Ư�� Űī ������ SaveThread �� �����.
// Ư�� Ű�� ������ ��� �����带 ���� ��Ŵ.
// ��� �����尡 ���� �Ǿ����� Ȯ�� �� ����.
void main()
{
	int count = 0;
	DWORD dwThreadID;
	HANDLE hThread[6];

	g_Shutdown_ManualResetEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	g_Save_autoResetEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	InitializeSRWLock(&datalist_srw);

	hThread[0] = (HANDLE)_beginthreadex(NULL, 0, PrintThread, NULL, 0, (unsigned int*)&dwThreadID);
	hThread[1] = (HANDLE)_beginthreadex(NULL, 0, DeleteThread, NULL, 0, (unsigned int*)&dwThreadID);
	hThread[2] = (HANDLE)_beginthreadex(NULL, 0, WorkerThread, NULL, 0, (unsigned int*)&dwThreadID);
	hThread[3] = (HANDLE)_beginthreadex(NULL, 0, WorkerThread, NULL, 0, (unsigned int*)&dwThreadID);
	hThread[4] = (HANDLE)_beginthreadex(NULL, 0, WorkerThread, NULL, 0, (unsigned int*)&dwThreadID);
	hThread[5] = (HANDLE)_beginthreadex(NULL, 0, SaveThread, NULL, 0, (unsigned int*)&dwThreadID);

	while (true)
	{
		//����ġ�� ������� ����
		if (GetAsyncKeyState(VK_RETURN))
		{
			SetEvent(g_Shutdown_ManualResetEvent);
			break;
		}

		//�����̽��� ġ�� ����
		if (GetAsyncKeyState(VK_SPACE))
		{
			SetEvent(g_Save_autoResetEvent);
		}

	}


	WaitForMultipleObjects(6, hThread, FALSE, INFINITE);


	system("pause");

}