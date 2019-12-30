#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

void main()
{
	int* p = (int*)malloc(100000000);
	
	while (1)
	{
		p++;
		*p = 0;
		Sleep(100);
	}
	
}