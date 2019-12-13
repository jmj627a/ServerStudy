#include <stdio.h>

void main()
{
	//0 ~ 255
	unsigned short num = 0;
	unsigned short temp = 0;

	bool bitFlag[16]{ 0, };

	int bitSit = 0;
	bool sitFlag = 0;

	while (true)
	{
		printf("자리 입력 >> ");
		scanf_s("%d", &bitSit);
		
		printf("OFF(0) , ON(1) >> ");
		scanf_s("%d", &sitFlag);

		num = temp;

		if (sitFlag == true)
			num |= (1 << bitSit);
		else
			num &= ~(1 << bitSit);


		temp = num;

		printf("\n");
		
		for (int i = 0; i <= 15; ++i)
		{
			bitFlag[i] = num % 2;
			num /= 2;
			printf("[ %d ] 번째 bit : %d \n", (i), bitFlag[i]);
		}


		printf("\n");
	}

}