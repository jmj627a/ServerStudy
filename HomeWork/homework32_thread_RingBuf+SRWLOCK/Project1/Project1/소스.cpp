#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <process.h>
#include <list>
#include <string>
#include "RingBuf.h"
#pragma comment(lib,"winmm")

struct st_MSG_HEAD
{
	short shType;
	short shStrLen;
};

//-메시지 타입
#define dfTYPE_ADD_STR 0
#define dfTYPE_DEL_STR 1
#define dfTYPE_PRINT_LIST 2
#define dfTYPE_QUIT 3


#define THREAD_NUM 3

// 컨텐츠 부, 문자열 리스트
std::list<std::wstring> g_List;
SRWLOCK srw_list;

// 스레드 메시지 큐 (사이즈 넉넉하게 크게 4~5만 바이트)
RingBuf g_msgQ;

//오토리셋 이벤트 - 스레드 풀링 방식은 다 깨어나면 의미가 없음 
HANDLE g_AutoResetEvent;

bool g_Shutdown = false;

const WCHAR *g_str = L"HELLOWORLD!";

//1초마다 List 에 rand 값 삽입.
unsigned __stdcall WorkerThread(void* arg)
{
	DWORD ThreadID = GetThreadId(GetCurrentThread());
	printf("Start WorkerThreadID : %d\n", ThreadID);

	while (!g_Shutdown) {

		//메인스레드 에서 메시지를 넣지 않았다면 쉬고 있어야 함.
		DWORD dwRet = WaitForSingleObject(g_AutoResetEvent, INFINITE);
		//if (dwRet != WAIT_TIMEOUT) 
		{
			st_MSG_HEAD mess;
			memset(&mess, 0, sizeof(st_MSG_HEAD));

			//메세지가 있는가? 
			g_msgQ.lock(WRITE);
			bool isData = g_msgQ.GetUseSize();

			if (isData)
			{
				g_msgQ.Dequeue((char *)&mess, sizeof(st_MSG_HEAD));
				g_msgQ.unlock(WRITE);
			}
			else 
			{
				g_msgQ.unlock(WRITE);
				continue;
			}

			switch (mess.shType) 
			{
				case dfTYPE_ADD_STR:
				{
					WCHAR str[40];
					wcsncpy_s(str, g_str, mess.shStrLen);
					AcquireSRWLockExclusive(&srw_list);
					g_List.push_back(str);
					ReleaseSRWLockExclusive(&srw_list);
				}
				break;
				case dfTYPE_DEL_STR:
				{
					AcquireSRWLockShared(&srw_list);
					if (!g_List.empty())
					{
						ReleaseSRWLockShared(&srw_list);

						AcquireSRWLockExclusive(&srw_list);
						g_List.pop_front();
						ReleaseSRWLockExclusive(&srw_list);
					}
					else
					{
						ReleaseSRWLockShared(&srw_list);

					}
				}
				break;
				case dfTYPE_PRINT_LIST:
				{
					std::list<std::wstring>::iterator iter;
					std::list<std::wstring>::iterator iterEnd = g_List.end();

					AcquireSRWLockShared(&srw_list);
					wprintf(L"List : ");
					for (iter = g_List.begin(); iter != iterEnd; ++iter) 
						wprintf(L"[%s] ", iter->c_str());
					wprintf(L"\n\n");
					ReleaseSRWLockShared(&srw_list);

				}
				break;
				case dfTYPE_QUIT:
				{
					g_Shutdown = true;
				}
				break;
			}
		}
	}

	printf("End WorkerThreadID : %d\n", ThreadID);

	return 0;
}


// 특정 키카 눌리면 SaveThread 를 깨운다.
// 특정 키가 눌리면 모든 스레드를 종료 시킴.
// 모든 스레드가 종료 되었는지 확인 후 꺼짐.
void main()
{

	int count = 0;
	DWORD dwThreadID;
	HANDLE hThread[THREAD_NUM];

	InitializeSRWLock(&srw_list);

	g_AutoResetEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	hThread[0] = (HANDLE)_beginthreadex(NULL, 0, WorkerThread, NULL, 0, (unsigned int*)&dwThreadID);
	hThread[1] = (HANDLE)_beginthreadex(NULL, 0, WorkerThread, NULL, 0, (unsigned int*)&dwThreadID);
	hThread[2] = (HANDLE)_beginthreadex(NULL, 0, WorkerThread, NULL, 0, (unsigned int*)&dwThreadID);

	static unsigned int time;
	static unsigned int delTime;

	while (true)
	{
		Sleep(50);
		st_MSG_HEAD mess;

		mess.shType = rand() % 3;
		mess.shStrLen = rand() % 11 + 1;

		//스페이스바 - 스레드들 종료
		if (GetAsyncKeyState(VK_SPACE))
		{
			mess.shType = dfTYPE_QUIT;
			mess.shStrLen = 0;
		}
		
	
		if (g_msgQ.DirectEnqueueSize() >= sizeof(st_MSG_HEAD))
		{
			g_msgQ.lock(WRITE);
			g_msgQ.Enqueue((char*)&mess, sizeof(st_MSG_HEAD));
			g_msgQ.unlock(WRITE);

			SetEvent(g_AutoResetEvent);
		}

		DWORD dwRet = WaitForMultipleObjects(THREAD_NUM, hThread, true, 0);
		if (dwRet != WAIT_TIMEOUT) 
			break;


		time = timeGetTime();

		if (time - delTime >= 1000)
		{
			delTime = time;
			g_msgQ.lock(READ);
			wprintf(L"***Queue using size : %d \n\n", g_msgQ.GetUseSize());
			g_msgQ.unlock(READ);
		}

	}


	for (int i = 0; i < THREAD_NUM; ++i)
		CloseHandle(hThread[i]);

	system("pause");

}