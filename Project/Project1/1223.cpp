#include <stdio.h>
#include <time.h>
#include <windows.h> 

#pragma comment(lib, "winmm.lib")

void main()
{
	while (1)
	{
		printf("%d \n", timeGetTime());
		
		//printf("%d \n", clock());
	}
}