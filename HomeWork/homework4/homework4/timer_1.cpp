#include <stdio.h>
#include <time.h>
#include <conio.h>
#include <Windows.h>
#include <string>

int g_nTiming[] = { 5, 10, 14, 17, 20, 25, 29, 31, 33 }; // 타이밍 간격
char g_szResult[9][10] = { 0, }; // 성공여부 표시
int g_nKeyCheck[9] = { 0, 0,0,0,0,0,0,0,0 }; // 타이머가 진행될 때 엔터키를 눌렀다면 그 누른 시간을 저장하는 용도
int g_nIndex = 0; // 기준 인덱스

int main()
{
	int nArrTimeCount = 9; // 총 타이밍 간격 갯수
	int nDiffTime = 0; // 타이밍 간격과 키보드 누른 시간의 차이를 저장하는 변수

	while (1)
	{
		// 현재 진행시간 
		clock_t NowTime = clock();

		system("cls");

		int nSec = NowTime / CLOCKS_PER_SEC;
		int nMilleSec = NowTime % CLOCKS_PER_SEC;

		// 타이밍 최고 시간을 넘거나 키보드 눌렀을 때 인덱스를 넘기면
		// 종료한다.
		if (nSec > g_nTiming[8] || g_nIndex > nArrTimeCount)
		{
			Sleep(1000);
			return 0;
		}

		if (_kbhit())
		{
			int a= _getch();
			// 키보드를 눌렀을 때 타이밍 시간을 저장한다. 
			g_nKeyCheck[g_nIndex] = NowTime;
			g_nIndex++;

		}

		// 타이밍 표시
		printf("%02d.%03d Sec \n\n", nSec, nMilleSec);

		for (int i = 0; i < nArrTimeCount; ++i)
		{

			// g_nKeyCheck에 키보드를 누른 시간이 0값이 아니고
			// g_nIndex보다 작으면 해당 키보드를 누른 값이 있다는 것.
			if (i < g_nIndex)
			{

				// 타이밍 간격과 키보드 누른시간의 차이를 구한다.
				nDiffTime = abs(g_nTiming[i] * CLOCKS_PER_SEC - g_nKeyCheck[i]);


				// 사용자가 키를 누르면 해당 시간을 체크하여 오차 범위에 따라서 
				// 지정 타이밍의 결과를 화면에 표시한다.
				// CLOCKS_PER_SEC / 4 한 수치를 1단계로 최대 4단계 까지의 오차로 
				// Great, Good, Nogood, Bad 단계 측정.
				if (nDiffTime <= 250 * 1)      strcpy_s(g_szResult[i], sizeof(g_szResult[i]), "GREAT");
				else if (nDiffTime <= 250 * 2)   strcpy_s(g_szResult[i], sizeof(g_szResult[i]), "GOOD");
				else if (nDiffTime <= 250 * 3)   strcpy_s(g_szResult[i], sizeof(g_szResult[i]), "NOGOOD");
				else if (nDiffTime <= 250 * 4)   strcpy_s(g_szResult[i], sizeof(g_szResult[i]), "BAD");
				else                     strcpy_s(g_szResult[i], sizeof(g_szResult[i]), "FAILE");
			}
			
			// 타이밍 간격 표시
			printf("%d Sec : %s\n", g_nTiming[i], g_szResult[i]);

		}
	}
} 