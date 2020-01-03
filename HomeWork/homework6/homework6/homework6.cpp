#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>

class CircleQueue
{
private:
	int arr[5]{ 0,};
	int front = 0;
	int rear = 0;

public:
	bool Enqueue(int indata)
	{
		//배열에 데이터 꽉 참
		if ((rear + 1) % 5 == front)
		{
			printf("배열이 꽉 찼습니다 \n");
			return false;
		}

		rear = (rear + 1) % 5;
		arr[rear] = indata;
		return true;
	}
	bool Dequeue(int& Outdata)
	{
		//배열이 비어있음
		if (front == rear)
		{
			printf("배열이 비어있습니다 \n");
			return false;
		}

		front = (front + 1) % 5;
		Outdata = arr[front];
		arr[front] = NULL;
		return true;
	}
	bool Peek(int& Outdata, int pos)
	{
		if (arr[pos] == NULL)
			return false;
	

		Outdata = arr[pos];

		return true;
	}


};
CircleQueue cQue;


void RenderGauge()
{
	printf("\n SPACE BAR로 생성 요청 \n\n");

	int beforeTime;
	for (int i = 0; i < 5; ++i)
	{
		if (cQue.Peek(beforeTime, i))
		{
			int delay = abs((int)(beforeTime - GetTickCount()));
			if (delay > 5000) delay = 5000; //출력될때 101% 되는게 싫어서
			printf("[ %d ] progress : %d %%  \n", i, delay/50);
		}
		else
			printf("[ %d ] progress : \n", i);

	}
	printf("\n");
}

void findOverGuage()
{
	int beforeTime;
	for (int i = 0; i < 5; ++i)
	{
		if (cQue.Peek(beforeTime, i))
		{
			int delay = abs((int)(beforeTime - GetTickCount()));
			if (delay >= 5000)
			{
				cQue.Dequeue(beforeTime);
				Sleep(10);
				printf("\n\n 유닛 생성 완료 !!!");
			}
		}
	}
}


void main()
{

	while (true)
	{
		Sleep(10);
		system("cls");

		RenderGauge();

		if (GetAsyncKeyState(VK_SPACE))
			cQue.Enqueue(GetTickCount());

		findOverGuage();
	}

}