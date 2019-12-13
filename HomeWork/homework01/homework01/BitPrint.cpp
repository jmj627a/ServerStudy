#include <stdio.h>

void main()
{
	//0 ~ 255
	unsigned char num = 100;
	unsigned char temp = num;

	bool bitFlag[8]{ 0, };


	for (int i = 7; i >= 0; --i)
	{
		bitFlag[i] = temp % 2;
		temp /= 2;
	}


	for (int i = 0; i < 8; ++i)
	{
		printf("%d", bitFlag[i]);
	}



}