#include <stdio.h>
#include <conio.h>
#include <stdlib.h>

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


void Gatcha()
{
//1. ��ü �����۵��� ���� �� ���� ����.
	int sum = 0;
	for (int i = 0; i < 7; ++i)
		sum += g_Gatcha[i].Rate;


// 2. rand() �Լ��� Ȯ���� ����
//���⼭ Ȯ���� 1/100 �� �ƴϸ�, 1/���պ��� ��.

// int iRand = Ȯ�����ϱ�;
	int iRand = rand() % sum;
	printf("%d \t", iRand);
// 3. ��ü ������ ���̺��� ���鼭
//������ ���� Rand ���� �ش� ������ �������� ã�´�.
	int range = 0;
	for (int i = 0; i < 7; ++i)
	{
		if (range <= iRand && iRand < g_Gatcha[i].Rate + range)
		{
			printf("%s \n", g_Gatcha[i].Name);
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
