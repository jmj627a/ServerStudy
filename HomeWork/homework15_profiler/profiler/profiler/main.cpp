#include <stdio.h>
#include <Windows.h>
#include "CProfile.h"

CProfile profile;


void main()
{
	for (int i = 0; i < 5; ++i)
	{
		profile.ProfileBegin("helloWorld");
		Sleep(1000);
		profile.ProfileEnd("helloWorld");
	}

	for (int i = 0; i < 5; ++i)
	{
		profile.ProfileBegin("helloWorld");
		Sleep(100);
		profile.ProfileEnd("helloWorld");
	}

	for (int i = 0; i < 3; ++i)
	{
		profile.ProfileBegin("I Like My Self");
		Sleep(1000);
		profile.ProfileEnd("I Like My Self");
	}

	profile.ProfileDataOutConsol();




	
	//LARGE_INTEGER Start;
	//LARGE_INTEGER End;
	//LARGE_INTEGER Freq;
	//QueryPerformanceFrequency(&Freq);	// 1���� �����ֱ� 1/(10000000)�� -> 0.0001ms����
	//
	//QueryPerformanceCounter(&Start);
	//Sleep(2000);
	//QueryPerformanceCounter(&End);
	

	
	//__int64 elapsed = End.QuadPart - Start.QuadPart;
	//double duringtime = (double)elapsed / (double)Freq.QuadPart;			//�ʴ���
	//double millisecond = duringtime * 1000;									//ms����


}