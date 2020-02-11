#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <time.h>
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

//Ư�� Ű�� ������ �ؽ�Ʈ ���Ϸ� ��µǰ�
void CProfile::ProfileDataOutText()// const char* szFileName)
{
	struct tm* today;
	time_t now = time(NULL);
	char file_name[100];
	FILE *fp;

	today = localtime(&now);  // ����ð�
	sprintf(file_name, "%d%02d%02d_%02d%02d%02d.txt", today->tm_year + 1900, today->tm_mon + 1, today->tm_mday
		, today->tm_hour, today->tm_min, today->tm_sec);

	fp = fopen(file_name, "w+");

	char buf[100];
	sprintf(buf," Name \t\t|\t Average \t\t|\t Min \t\t|\t Max \t\t|\t Call \n");
	fprintf(fp, buf);

	for (int i = 0; i < 10; ++i)
	{
		if (false == data[i].iFlag)
			continue;

		sprintf(buf, "%s \t|\t %0.4lf�� \t|\t %0.4lf�� \t|\t %0.4lf�� \t|\t %d \n",
			data[i].szName, (data[i].iTotalTime - data[i].iMax - data[i].iMin) / (data[i].iCall - 2), data[i].iMin, data[i].iMax, data[i].iCall - 2);
		fprintf(fp, buf);
	}

	fclose(fp);
}

//Ư�� Ű�� ������ �ܼ� ��µǰ�
void CProfile::ProfileDataOutConsol()
{
	printf(" Name \t\t|\t Average \t|\t Min \t\t|\t Max \t\t|\t Call \n");

	for (int i = 0; i < 10; ++i)
	{
		if (false == data[i].iFlag)
			continue;

		printf("%s \t|\t %0.4lf�� \t|\t %0.4lf�� \t|\t %0.4lf�� \t|\t %d \n",
			data[i].szName, (data[i].iTotalTime- data[i].iMax- data[i].iMin) / (data[i].iCall - 2), data[i].iMin, data[i].iMax, data[i].iCall-2);
	}
}

void CProfile::ProfileReset(void)
{
	//Ư�� Ű�� ������ ������ ���� �ʱ�ȭ. 
	//������ ���� �� ���ʵ����� ���� �ǹ� ���� �����Ͱ� ���Ƿ� 
	//���� �Ǳ� ������ �������� �����͸� ������ ���� Ű�� ������ ���µǵ���.
	for (int i = 0; i < 10; ++i)
	{
		//data[i].iFlag = false;
		//data[i].szName[64] = NULL;
		//data[i].iStartTime = ;
		data[i].iTotalTime = 0;
		data[i].iMin = 0x7fffffffffffffff;
		data[i].iMax = 0;
		data[i].iCall = 0;
	}
}
