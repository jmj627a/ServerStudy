#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include "CProfile.h"

void CProfile::ProfileBegin(const char* szName)
{
	//�迭�� ���� �̹� �ִ� nameTag���� Ȯ��
	for (int i = 0; i < 10; ++i)
	{
		//�̹� �ִٸ� call Ƚ�� + 1
		if (0 == strcmp((const char*)data[i].szName, (const char*)szName))
		{
			data[i].iCall++;
			QueryPerformanceCounter(&Start);
			return;
		}
	}

	//���ٸ� �迭�� ���� �ֱ�
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

	//�迭�� ���� �̹� �ִ� nameTag���� Ȯ��  
	//�ٵ� �̹� Begin���� �����ϱ� ã�Ƽ� �ִ� �۾�
	for (int i = 0; i < 10; ++i)
	{
		if (0 == strcmp(data[i].szName, szName))
		{
			//�ð� ����
			__int64 elapsed = End.QuadPart - Start.QuadPart;
			data[i].iTotalTime += ((double)elapsed / (double)Freq.QuadPart);// ms�� �ٲٴ°� ��¶� *1000;

			//�ִ� �ּ� ����
			if (data[i].iMax < (double)elapsed / (double)Freq.QuadPart)	
				data[i].iMax = (double)elapsed / (double)Freq.QuadPart;
			if (data[i].iMin > (double)elapsed / (double)Freq.QuadPart)	
				data[i].iMin = (double)elapsed / (double)Freq.QuadPart;
		}
	}
}

void CProfile::ProfileDataOutText(const char* szFileName)
{
	//Ư�� Ű�� ������ �ؽ�Ʈ ���Ϸ� ��µǰ�
	for (int i = 0; i < 10; ++i)
	{
		printf("%s \t|\t %0.4lf�� \t|\t %0.4lf�� \t|\t %0.4lf�� \t|\t %d \n", 
			data[i].szName, (data[i].iTotalTime*1000)/(data[i].iCall-1), data[i].iMin, data[i].iMax, data[i].iCall);
	}
}

void CProfile::ProfileDataOutConsol()
{
	//Ư�� Ű�� ������ �ؽ�Ʈ ���Ϸ� ��µǰ�
	for (int i = 0; i < 10; ++i)
	{
		if (false == data[i].iFlag)
			continue;

		printf("%s \t|\t %0.4lf�� \t|\t %0.4lf�� \t|\t %0.4lf�� \t|\t %d \n",
			data[i].szName, (data[i].iTotalTime) / (data[i].iCall - 1), data[i].iMin, data[i].iMax, data[i].iCall);
	}
}

void CProfile::ProfileReset(void)
{
	//Ư�� Ű�� ������ ������ ���� �ʱ�ȭ. 
	//������ ���� �� ���ʵ����� ���� �ǹ� ���� �����Ͱ� ���Ƿ� 
	//���� �Ǳ� ������ �������� �����͸� ������ ���� Ű�� ������ ���µǵ���.
}
