#include <stdio.h>
#include <Windows.h>
#include "CProfile.h"

CProfile profile;
bool flag = true;

void main()
{

	while (true == flag)
	{
		profile.ProfileBegin("helloWorld");
		Sleep(2000);
		profile.ProfileEnd("helloWorld");

		profile.ProfileBegin("helloWorld");
		Sleep(100);
		profile.ProfileEnd("helloWorld");

		profile.ProfileBegin("I Like My Self!");
		Sleep(1000);
		profile.ProfileEnd("I Like My Self!");

		if(GetAsyncKeyState(VK_F1))
			profile.ProfileDataOutText();
		if (GetAsyncKeyState(VK_F2))
			profile.ProfileDataOutConsol();
		if (GetAsyncKeyState(VK_F3))
			profile.ProfileReset();
		if (GetAsyncKeyState(VK_F12))
			flag = false;
	}



	
	//LARGE_INTEGER Start;
	//LARGE_INTEGER End;
	//LARGE_INTEGER Freq;
	//QueryPerformanceFrequency(&Freq);	// 1초의 진동주기 1/(10000000)초 -> 0.0001ms단위
	//
	//QueryPerformanceCounter(&Start);
	//Sleep(2000);
	//QueryPerformanceCounter(&End);
	

	
	//__int64 elapsed = End.QuadPart - Start.QuadPart;
	//double duringtime = (double)elapsed / (double)Freq.QuadPart;			//초단위
	//double millisecond = duringtime * 1000;									//ms단위


}