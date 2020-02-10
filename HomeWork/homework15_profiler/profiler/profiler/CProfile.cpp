#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include "CProfile.h"

void CProfile::ProfileBegin(const char* szName)
{
	//배열을 돌며 이미 있는 nameTag인지 확인
	for (int i = 0; i < 10; ++i)
	{
		//이미 있다면 call 횟수 + 1
		if (0 == strcmp((const char*)data[i].szName, (const char*)szName))
		{
			data[i].iCall++;
			QueryPerformanceCounter(&Start);
			return;
		}
	}

	//없다면 배열에 새로 넣기
	for (int i = 0; i < 10; ++i)
	{
		if (data[i].iFlag == false)
		{
			data[i].iFlag = true;
			data[i].iCall++;
			
			strcpy(data[i].szName, szName);
			QueryPerformanceCounter(&Start);
			data[i].iStartTime = Start;

			return;
		}
	}
}

void CProfile::ProfileEnd(const char* szName)
{
	QueryPerformanceCounter(&End);

	//배열을 돌며 이미 있는 nameTag인지 확인  
	//근데 이미 Begin에서 했으니까 찾아서 넣는 작업
	for (int i = 0; i < 10; ++i)
	{
		if (0 == strcmp(data[i].szName, szName))
		{
			//시간 누적
			__int64 elapsed = End.QuadPart - Start.QuadPart;
			data[i].iTotalTime += ((double)elapsed / (double)Freq.QuadPart);// ms로 바꾸는건 출력때 *1000;

			//최대 최소 갱신
			if (data[i].iMax < (double)elapsed / (double)Freq.QuadPart)	
				data[i].iMax = (double)elapsed / (double)Freq.QuadPart;
			if (data[i].iMin > (double)elapsed / (double)Freq.QuadPart)	
				data[i].iMin = (double)elapsed / (double)Freq.QuadPart;
		}
	}
}

void CProfile::ProfileDataOutText(const char* szFileName)
{
	//특정 키를 누르면 텍스트 파일로 출력되게
	for (int i = 0; i < 10; ++i)
	{
		printf("%s \t|\t %0.4lf㎲ \t|\t %0.4lf㎲ \t|\t %0.4lf㎲ \t|\t %d \n", 
			data[i].szName, (data[i].iTotalTime*1000)/(data[i].iCall-1), data[i].iMin, data[i].iMax, data[i].iCall);
	}
}

void CProfile::ProfileDataOutConsol()
{
	//특정 키를 누르면 텍스트 파일로 출력되게
	for (int i = 0; i < 10; ++i)
	{
		if (false == data[i].iFlag)
			continue;

		printf("%s \t|\t %0.4lf㎲ \t|\t %0.4lf㎲ \t|\t %0.4lf㎲ \t|\t %d \n",
			data[i].szName, (data[i].iTotalTime) / (data[i].iCall - 1), data[i].iMin, data[i].iMax, data[i].iCall);
	}
}

void CProfile::ProfileReset(void)
{
	//특정 키를 누르면 네임택 빼고 초기화. 
	//실제로 시작 후 몇초동안은 수집 의미 없는 데이터가 들어가므로 
	//재사용 되기 시작한 순간부터 데이터를 모으기 위해 키를 누르면 리셋되도록.
}
