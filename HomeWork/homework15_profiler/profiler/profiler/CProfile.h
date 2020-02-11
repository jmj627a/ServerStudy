#pragma once

#include <Windows.h>

struct PROFILE_DATA
{
	bool			iFlag;				// 프로파일의 사용 여부. (배열시에만)
	char			szName[64];			// 프로파일 샘플 이름.
	//WCHAR			szName[64];			// 프로파일 샘플 이름.

	LARGE_INTEGER	iStartTime;			// 프로파일 샘플 실행 시간.

	double			iTotalTime;			// 전체 사용시간 카운터 Time.	(출력시 호출회수로 나누어 평균 구함)
	double			iMin;				// 최소 사용시간 카운터 Time.	(초단위로 계산하여 저장 / [0] 가장최소 [1] 다음 최소 [2])
	double			iMax;				// 최대 사용시간 카운터 Time.	(초단위로 계산하여 저장 / [0] 가장최대 [1] 다음 최대 [2])

	__int64			iCall;				// 누적 호출 횟수.
};



class CProfile
{
public:
	LARGE_INTEGER Start;
	LARGE_INTEGER End;
	LARGE_INTEGER Freq;

	PROFILE_DATA data[10];

	void initProfile()
	{
		QueryPerformanceFrequency(&Freq);

		for (int i = 0; i < 10; ++i)
		{
			data[i].iFlag = false;
			data[i].szName[64] = NULL;
			//data[i].iStartTime = ;
			data[i].iTotalTime = 0;
			data[i].iMin = 0x7fffffffffffffff;
			data[i].iMax = 0;
			data[i].iCall = 0;
		}
	}


	/////////////////////////////////////////////////////////////////////////////
	// 하나의 함수 Profiling 시작, 끝 함수.
	//
	// Parameters: (char *)Profiling이름.
	// Return: 없음.
	/////////////////////////////////////////////////////////////////////////////
	void ProfileBegin(const char* szName);
	void ProfileEnd(const char* szName);

	/////////////////////////////////////////////////////////////////////////////
	// Profiling 된 데이타를 Text 파일로 출력한다.
	//
	// Parameters: (char *)출력될 파일 이름.
	// Return: 없음.
	/////////////////////////////////////////////////////////////////////////////
	void ProfileDataOutText();// const char* szFileName);

	void ProfileDataOutConsol();

	/////////////////////////////////////////////////////////////////////////////
	// 프로파일링 된 데이터를 모두 초기화 한다.
	//
	// Parameters: 없음.
	// Return: 없음.
	/////////////////////////////////////////////////////////////////////////////
	void ProfileReset(void);


	CProfile()
	{
		initProfile();
	}

	~CProfile()
	{

	}

};

