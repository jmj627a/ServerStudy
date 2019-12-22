#include <stdio.h>
#include <conio.h>
#include <stdlib.h>

struct st_ITEM
{
	char Name[30];
	int Rate; // �Ϲ� ���� �̱�� ���� �뵵
	int WinTime; // �� �������� ���� �̱� ȸ��.
	// 0 �̸� �Ϲ� ������
	// 0 �� �ƴϸ� �� ȸ������ ����
};

st_ITEM g_Gatcha[] = {
{"Į", 20, 0},
{"����", 20, 0},
{"�Ź�", 20, 0},
{"����", 20, 0},
{"�ʰ��·����", 5, 0},
{"�ʰ��¹���", 5, 0},
{"�ʰ��·�����Ź�1", 1, 50},
{"�ʰ��·�����Ź�2", 1, 51},
{"�ʰ��·�����Ź�3", 1, 10}

// ������ 3���� �������� �Ϲ� Ȯ���δ� ������ ������
// �ڿ� �Էµ� WinTime ȸ������ 100% �� ����.
};

// �̱��� ȸ���� ���� ����� �ǰ� �־�� ��.
int iCount = 0;


void Gatcha()
{
// 1. ��ü �����۵��� ���� �� ���� ����.
	int sum = 0;
	for (int i = 0; i < 6; ++i) //�ڿ� 3���� Ȯ������ ����
		sum += g_Gatcha[i].Rate;

	int iRand = rand() % sum;

// �̱� ȸ�� ����. (�̴� ���������� ��� �Ǿ�� ��)
	iCount++;
	printf("count : %d, \t %d \t",iCount, iRand);

// 2. �� �̱� ȸ���� ���� ���� �������� �ִ��� Ȯ��
	for (int i = 0; i < 9; ++i)
	{
		if (iCount == g_Gatcha[i].WinTime)
		{
			printf("%s \n", g_Gatcha[i].Name);
			return;
		}
	}
// 3. rand() �Լ��� Ȯ���� ����


// 4. ��ü ������ ���̺��� ���鼭 ������ ���� Rand ���� �ش� ������ �������� ã�´�.
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

// 5. �̱� ȸ���� �ʱ�ȭ �ؾ����� �Ǵ��Ͽ� �ʱ�ȭ.
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