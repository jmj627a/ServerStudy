#include <stdio.h>
#include <time.h>
#include <conio.h>
#include <Windows.h>
#include <string>

int g_nTiming[] = { 5, 10, 14, 17, 20, 25, 29, 31, 33 }; // Ÿ�̹� ����
char g_szResult[9][10] = { 0, }; // �������� ǥ��
int g_nKeyCheck[9] = { 0, 0,0,0,0,0,0,0,0 }; // Ÿ�̸Ӱ� ����� �� ����Ű�� �����ٸ� �� ���� �ð��� �����ϴ� �뵵
int g_nIndex = 0; // ���� �ε���

int main()
{
	int nArrTimeCount = 9; // �� Ÿ�̹� ���� ����
	int nDiffTime = 0; // Ÿ�̹� ���ݰ� Ű���� ���� �ð��� ���̸� �����ϴ� ����

	while (1)
	{
		// ���� ����ð� 
		clock_t NowTime = clock();

		system("cls");

		int nSec = NowTime / CLOCKS_PER_SEC;
		int nMilleSec = NowTime % CLOCKS_PER_SEC;

		// Ÿ�̹� �ְ� �ð��� �Ѱų� Ű���� ������ �� �ε����� �ѱ��
		// �����Ѵ�.
		if (nSec > g_nTiming[8] || g_nIndex > nArrTimeCount)
		{
			Sleep(1000);
			return 0;
		}

		if (_kbhit())
		{
			int a= _getch();
			// Ű���带 ������ �� Ÿ�̹� �ð��� �����Ѵ�. 
			g_nKeyCheck[g_nIndex] = NowTime;
			g_nIndex++;

		}

		// Ÿ�̹� ǥ��
		printf("%02d.%03d Sec \n\n", nSec, nMilleSec);

		for (int i = 0; i < nArrTimeCount; ++i)
		{

			// g_nKeyCheck�� Ű���带 ���� �ð��� 0���� �ƴϰ�
			// g_nIndex���� ������ �ش� Ű���带 ���� ���� �ִٴ� ��.
			if (i < g_nIndex)
			{

				// Ÿ�̹� ���ݰ� Ű���� �����ð��� ���̸� ���Ѵ�.
				nDiffTime = abs(g_nTiming[i] * CLOCKS_PER_SEC - g_nKeyCheck[i]);


				// ����ڰ� Ű�� ������ �ش� �ð��� üũ�Ͽ� ���� ������ ���� 
				// ���� Ÿ�̹��� ����� ȭ�鿡 ǥ���Ѵ�.
				// CLOCKS_PER_SEC / 4 �� ��ġ�� 1�ܰ�� �ִ� 4�ܰ� ������ ������ 
				// Great, Good, Nogood, Bad �ܰ� ����.
				if (nDiffTime <= 250 * 1)      strcpy_s(g_szResult[i], sizeof(g_szResult[i]), "GREAT");
				else if (nDiffTime <= 250 * 2)   strcpy_s(g_szResult[i], sizeof(g_szResult[i]), "GOOD");
				else if (nDiffTime <= 250 * 3)   strcpy_s(g_szResult[i], sizeof(g_szResult[i]), "NOGOOD");
				else if (nDiffTime <= 250 * 4)   strcpy_s(g_szResult[i], sizeof(g_szResult[i]), "BAD");
				else                     strcpy_s(g_szResult[i], sizeof(g_szResult[i]), "FAILE");
			}
			
			// Ÿ�̹� ���� ǥ��
			printf("%d Sec : %s\n", g_nTiming[i], g_szResult[i]);

		}
	}
} 