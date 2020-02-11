#pragma once

#include <Windows.h>

struct PROFILE_DATA
{
	bool			iFlag;				// ���������� ��� ����. (�迭�ÿ���)
	char			szName[64];			// �������� ���� �̸�.
	//WCHAR			szName[64];			// �������� ���� �̸�.

	LARGE_INTEGER	iStartTime;			// �������� ���� ���� �ð�.

	double			iTotalTime;			// ��ü ���ð� ī���� Time.	(��½� ȣ��ȸ���� ������ ��� ����)
	double			iMin;				// �ּ� ���ð� ī���� Time.	(�ʴ����� ����Ͽ� ���� / [0] �����ּ� [1] ���� �ּ� [2])
	double			iMax;				// �ִ� ���ð� ī���� Time.	(�ʴ����� ����Ͽ� ���� / [0] �����ִ� [1] ���� �ִ� [2])

	__int64			iCall;				// ���� ȣ�� Ƚ��.
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
	// �ϳ��� �Լ� Profiling ����, �� �Լ�.
	//
	// Parameters: (char *)Profiling�̸�.
	// Return: ����.
	/////////////////////////////////////////////////////////////////////////////
	void ProfileBegin(const char* szName);
	void ProfileEnd(const char* szName);

	/////////////////////////////////////////////////////////////////////////////
	// Profiling �� ����Ÿ�� Text ���Ϸ� ����Ѵ�.
	//
	// Parameters: (char *)��µ� ���� �̸�.
	// Return: ����.
	/////////////////////////////////////////////////////////////////////////////
	void ProfileDataOutText();// const char* szFileName);

	void ProfileDataOutConsol();

	/////////////////////////////////////////////////////////////////////////////
	// �������ϸ� �� �����͸� ��� �ʱ�ȭ �Ѵ�.
	//
	// Parameters: ����.
	// Return: ����.
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

