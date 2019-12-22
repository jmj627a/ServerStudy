#include <stdio.h>
#include <conio.h>
#include <stdlib.h>

struct st_ITEM
{
	char Name[30];
	int Rate; // 일반 랜덤 뽑기와 같은 용도
	int WinTime; // 이 아이템이 나올 뽑기 회차.
	// 0 이면 일반 아이템
	// 0 이 아니면 그 회차에만 나옴
};

st_ITEM g_Gatcha[] = {
{"칼", 20, 0},
{"방패", 20, 0},
{"신발", 20, 0},
{"물약", 20, 0},
{"초강력레어무기", 5, 0},
{"초강력방패", 5, 0},
{"초강력레어레어레어신발1", 1, 50},
{"초강력레어레어레어신발2", 1, 51},
{"초강력레어레어레어신발3", 1, 10}

// 마지막 3개의 아이템은 일반 확률로는 나오지 않으며
// 뒤에 입력된 WinTime 회차때만 100% 로 나옴.
};

// 뽑기의 회차가 누적 계산이 되고 있어야 함.
int iCount = 0;


void Gatcha()
{
// 1. 전체 아이템들의 비율 총 합을 구함.
	int sum = 0;
	for (int i = 0; i < 6; ++i) //뒤에 3개는 확률에서 제외
		sum += g_Gatcha[i].Rate;

	int iRand = rand() % sum;

// 뽑기 회차 증가. (이는 전역적으로 계산 되어야 함)
	iCount++;
	printf("count : %d, \t %d \t",iCount, iRand);

// 2. 본 뽑기 회차에 대한 지정 아이템이 있는지 확인
	for (int i = 0; i < 9; ++i)
	{
		if (iCount == g_Gatcha[i].WinTime)
		{
			printf("%s \n", g_Gatcha[i].Name);
			return;
		}
	}
// 3. rand() 함수로 확률을 구함


// 4. 전체 아이템 테이블을 돌면서 위에서 구한 Rand 값에 해당 지점의 아이템을 찾는다.
	int range = 0;
	for (int i = 0; i < 6; ++i)
	{
		if (range <= iRand && iRand < g_Gatcha[i].Rate + range)
		{
			printf("%s \n", g_Gatcha[i].Name);
			break;
		}
		else
			range += g_Gatcha[i].Rate;
	}

// 5. 뽑기 회차를 초기화 해야할지 판단하여 초기화.
	if (iCount > 93)
		iCount = 0;
}

int main()
{
	while (1)
	{
		_getch();
		Gatcha();
	}
	return 0;
}