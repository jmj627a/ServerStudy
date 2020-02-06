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

//Work�� Thread ������ ���� ����ü
struct __ThreadPool
{
	//Work�� ����ϱ� ���� �迭
	WORK workList[WORK_MAX];

	//Thread ������ �� Thread�� Event ������Ʈ
	WorkerThread workerThreadList[THREAD_MAX];
	HANDLE workerEventList[THREAD_MAX];

	//Work�� ���� ����
	DWORD idxOfCurrentWork; // ��� 1���� Work Index
	DWORD idxOfLastAddedWork; // ������ �߰� Work index + 1

	//Number of Thread
	DWORD threadIdx;// Pool �� �����ϴ� Thread�� ����

} gThreadPool;

