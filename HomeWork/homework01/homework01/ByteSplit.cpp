#include <stdio.h>

void main()
{
	unsigned int number = 0;
	int sit;
	unsigned int value[4]{ 0, };
	int index;

	while (true)
	{
		printf("��ġ (1 ~ 4) : ");
		scanf_s("%d", &sit);
		index = sit - 1;

		printf("�� [0 ~ 255] : ");
		scanf_s("%d", &value[index]);
		 
		for (int i = 0; i < 4; ++i)
		{
			printf("%d ��° ����Ʈ �� : %d \n", (i+1) , value[i]);
		}

		number &= ~(0xFF << (index * 8));
		number |= value[index] << (index * 8);

		//16���� ��� 
		printf("\n0x%p	\n\n", number);
	}

}