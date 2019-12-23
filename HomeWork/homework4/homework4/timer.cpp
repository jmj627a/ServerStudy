#include <stdio.h>
#include <time.h>
#include <conio.h>
#include <Windows.h>
#include <string>

int g_Timing[] = { 5, 10, 14, 17, 20, 25, 29, 31, 33 , 40};
int myTimer[10] = { 0, };

int timeCapture = 0;
int index = 0;

const char* timeCheck(int n)
{
	int num = abs((g_Timing[n]*1000) - myTimer[n]);

	//1�ʰ� �Ѱ� ���̳����� �Է��� ������ fail
	if (myTimer[n] == 0)
	{
		if (clock() - g_Timing[n] * 1000 > 1000)
		{
			myTimer[n] = clock();
			index++;
		}
	}

	if (0 <= num && num < 100)
		return "Excellent";
	else if (100 <= num && num < 500)
		return "Great";
	else if (500 <= num && num < 1000)
		return "Bad";
	else if (myTimer[n] == 0)
		return "No Data";
	else
		return "Fail";

}


void main()
{
	while (true)
	{
		Sleep(10);
		system("cls");

		timeCapture = clock();
		
		if (_kbhit())
		{
			_getch();
			myTimer[index] = timeCapture;
			index++;
		}

		//����ð� ���
		printf("%03d:%03d sec \n\n", timeCapture / 1000, timeCapture % 1000);

		//�迭�� ����� ���� ��� ������ �ִ��� ��
		for (int i = 0; i < 10; ++i)
		{
			printf("%d. %03d sec : %s \t(delay : %d)\n", 
				i, g_Timing[i], timeCheck(i), abs((g_Timing[i] * 1000) - myTimer[i]));
		}
	}
}