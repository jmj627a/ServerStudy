#include <stdio.h>
#include <stdlib.h>

class Test
{
	int a;
	int b; 
	char c;

public:

	Test() {};

};

void main()
{
	void* temp = new Test();

	void* temp1 = malloc(sizeof(Test));
}

typedef void(*WORK)(void);

typedef struct __WorkerThread

{
	HANDLE hThread;
	DWORD idThread;

} WorkerThread;

//Work와 Thread 관리를 위한 구조체
struct __ThreadPool
{
	//Work를 등록하기 위한 배열
	WORK workList[WORK_MAX];

	//Thread 정보와 각 Thread별 Event 오브젝트
	WorkerThread workerThreadList[THREAD_MAX];
	HANDLE workerEventList[THREAD_MAX];

	//Work에 대한 정보
	DWORD idxOfCurrentWork; // 대기 1순위 Work Index
	DWORD idxOfLastAddedWork; // 마지막 추가 Work index + 1

	//Number of Thread
	DWORD threadIdx;// Pool 에 존재하는 Thread의 개수

} gThreadPool;

