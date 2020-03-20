#include <stdio.h>
#include <Windows.h>
#pragma comment(lib, "winmm.lib") 
int cur = 0;
int old = 0;
int deltaTime;

void main()
{
	timeBeginPeriod(1);

	while (true)
	{
		static int cur = 0;
		static int old = 0;
		static int deltaTime;

		cur = timeGetTime();
		deltaTime = cur - old;

		if (deltaTime < 20)
		{
			printf("sleep : %d \n", 20 - deltaTime);
			Sleep(20 - deltaTime);
		}
		old = cur + (20 - deltaTime);


		static wchar_t szFrame[5];
		static int iFrame = 0;
		static DWORD dwTick = 0;

		iFrame++;

		if (dwTick + 1000 < timeGetTime())
		{
			printf("==============frame : %d \n", iFrame);
			iFrame = 0;
			dwTick = timeGetTime();
		}

	}
}