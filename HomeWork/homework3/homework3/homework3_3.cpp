#include <stdio.h>
#include <conio.h>
#include <stdlib.h>


int RandTable[91];  // 랜덤 범위를 최대 1000 개로 잡음.

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



int TableRand(int iMax)
{
	static int RandTableIndex = 0;

	// 랜덤 뽑을때마다 뽑혀진 랜덤 값을
	// 모두 저장해야 하므로 저장 위치 배열의 인덱스를 보관 하여 둔다.

	//RandTableIndex == 0 이면 RandTable 초기화.
	if (RandTableIndex == 0)
	{
		for (int i = 0; i < iMax; ++i)
			RandTable[i] = 0;
	}

	do
	{
		//1. 일반 랜덤 rand() 으로 랜덤 값 구함.
		int randomIndex = rand() % iMax;

		//2. RandTable 에 저장된 값 중 같은값이 있는지 전체를 돌면서 확인.
		while (true)
		{
			int key = true;
			for (int i = 0; i < RandTableIndex; ++i)
			{
				if (RandTable[i] == randomIndex)
				{
					key = false;
					break;
				}
			}

			if (key == true)
			{
				//3. RandTable 에 같은값이 없다면 방금 뽑은 랜덤값을 RandTable 에 추가저장.
				RandTable[RandTableIndex] = randomIndex;
				RandTableIndex++;


				int range = 0;
				for (int i = 0; i < 7; ++i)
				{
					if (range <= randomIndex && randomIndex < g_Gatcha[i].Rate + range)
					{
						printf("index[%d] \t %s \n", randomIndex, g_Gatcha[i].Name);
						break;
					}
					else
						range += g_Gatcha[i].Rate;
				}



				return randomIndex;
			}
			else
				randomIndex = rand() % iMax;


			if (RandTableIndex >= iMax)
			{
				RandTableIndex = 0;
				printf("-----------------------------------------------------------------------------\n");
			}
		}



	} while (0);

}

int main()
{
	while(true)
	{ 
		TableRand(91);

		_getch();
	}
	
	
	return 0;
}