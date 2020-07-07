/*
DATA : STL List < 숫자 >


MainThread

		특정 키카 눌리면 SaveThread 를 깨운다.
		특정 키가 눌리면 모든 스레드를 종료 시킴.
		모든 스레드가 종료 되었는지 확인 후 꺼짐.


PrintThread


		1초마다 List 의 데이터를 출력

		 1-2-59-20-39-9-192-85-2-385-282-2-3- ....


DeleteThread

		  333ms 마다 List 의 끝 데이터를 삭제


WorkerThread x 3

		  1초마다 List 에 rand 값 삽입.


SaveThread

		  메인스레드의 이벤트에 의해 깨어나며
		  깨어나면 List 의 내용을 파일 TXT 로 저장한다.
		  (2회 이상 저장시 기존 파일을 지우고 저장한다)

		  조건 : Sleep 을 쓰지 않음.
*/

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <process.h>
#include <list>

//이벤트 - save 이벤트 - auto리셋이벤트
HANDLE g_Save_autoResetEvent = false;

//이벤트 - 종료 이벤트 - manual리셋이벤트
HANDLE g_Shutdown_ManualResetEvent = false;

//list
std::list<int> g_dataList;

//list 용 SRWLock
SRWLOCK datalist_srw;

//1초마다 List 의 데이터를 출력
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

//333ms 마다 List 의 끝 데이터를 삭제
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

//1초마다 List 에 rand 값 삽입.
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

//메인스레드의 이벤트에 의해 깨어나며
//깨어나면 List 의 내용을 파일 TXT 로 저장한다.
//(2회 이상 저장시 기존 파일을 지우고 저장한다)
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

// 특정 키카 눌리면 SaveThread 를 깨운다.
// 특정 키가 눌리면 모든 스레드를 종료 시킴.
// 모든 스레드가 종료 되었는지 확인 후 꺼짐.
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
		//엔터치면 스레드들 종료
		if (GetAsyncKeyState(VK_RETURN))
		{
			SetEvent(g_Shutdown_ManualResetEvent);
			break;
		}

		//스페이스바 치면 저장
		if (GetAsyncKeyState(VK_SPACE))
		{
			SetEvent(g_Save_autoResetEvent);
		}

	}


	WaitForMultipleObjects(6, hThread, FALSE, INFINITE);


	system("pause");

}