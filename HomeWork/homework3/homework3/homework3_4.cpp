#include <stdio.h>
#include <conio.h>
#include <stdlib.h>

struct st_ITEM
{
	char Name[30];
	int Rate; // 아이템이 뽑힐 비율
};

st_ITEM g_Gatcha[] = {
	{"칼", 20},
	{"방패", 20},
	{"신발", 20},
	{"물약", 20},
	{"초강력레어무기", 5},
	{"초강력방패", 5},
	{"초강력레어레어레어신발", 1}
};


void Gatcha()
{
	int sum = 0;
	for (int i = 0; i < 7; ++i)
		sum += g_Gatcha[i].Rate;

	if (sum <= 0)
	{
		printf("모든 아이템이 나올 확률이 0입니다 \n");
		return;
	}

	int iRand = rand() % sum;
	printf("%d \t", iRand);

	int range = 0;
	for (int i = 0; i < 7; ++i)
	{
		if (range <= iRand && iRand < g_Gatcha[i].Rate + range)
		{
			printf("%s \n", g_Gatcha[i].Name);
			g_Gatcha[i].Rate--;
			break;
		}
		else
			range += g_Gatcha[i].Rate;
	}

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
