#include <stdio.h>
#include <math.h>
#include <Windows.h>

int main()
{
	int iAngle = 0;
	double dRadian = 0;
	double num = 0;

	while (true)
	{
		iAngle += 5;
		if (iAngle >= 180)
			iAngle = 0;

		dRadian = iAngle * 3.14 / 180;
		num = sin(dRadian) * 100;

		for (int i = 0; i < num; ++i)
		{
			printf("*");
		}
		printf("\n");

		Sleep(50);
	}


	return 0;
}