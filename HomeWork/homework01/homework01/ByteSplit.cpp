#include <stdio.h>

void main()
{
	unsigned int number = 0;
	int sit;
	unsigned int value[4]{ 0, };
	int index;

	while (true)
	{
		printf("위치 (1 ~ 4) : ");
		scanf_s("%d", &sit);
		index = sit - 1;

		printf("값 [0 ~ 255] : ");
		scanf_s("%d", &value[index]);
		 
		for (int i = 0; i < 4; ++i)
		{
			printf("%d 번째 바이트 값 : %d \n", (i+1) , value[i]);
		}

		number &= ~(0xFF << (index * 8));
		number |= value[index] << (index * 8);

		//16진수 출력 
		printf("\n0x%p	\n\n", number);
	}

}