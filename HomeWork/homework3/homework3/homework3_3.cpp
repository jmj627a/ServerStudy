#include <stdio.h>
#include <conio.h>
#include <stdlib.h>


int RandTable[91];  // ���� ������ �ִ� 1000 ���� ����.

struct st_ITEM
{
	char Name[30];
	int Rate; // �������� ���� ����
};

st_ITEM g_Gatcha[] = {
	{"Į", 20},
	{"����", 20},
	{"�Ź�", 20},
	{"����", 20},
	{"�ʰ��·����", 5},
	{"�ʰ��¹���", 5},
	{"�ʰ��·�����Ź�", 1}
};



int TableRand(int iMax)
{
	static int RandTableIndex = 0;

	// ���� ���������� ������ ���� ����
	// ��� �����ؾ� �ϹǷ� ���� ��ġ �迭�� �ε����� ���� �Ͽ� �д�.

	//RandTableIndex == 0 �̸� RandTable �ʱ�ȭ.
	if (RandTableIndex == 0)
	{
		for (int i = 0; i < iMax; ++i)
			RandTable[i] = 0;
	}

	do
	{
		//1. �Ϲ� ���� rand() ���� ���� �� ����.
		int randomIndex = rand() % iMax;

		//2. RandTable �� ����� �� �� �������� �ִ��� ��ü�� ���鼭 Ȯ��.
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
				//3. RandTable �� �������� ���ٸ� ��� ���� �������� RandTable �� �߰�����.
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