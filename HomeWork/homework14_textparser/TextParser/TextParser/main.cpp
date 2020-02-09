#include <stdio.h>
#include "CParse.h"

void main()
{
	CParse parser;
	int iValue1, iValue2, iValue3, iValue4;

	parser.LoadFile("info.txt");

	parser.GetValue("Version", &iValue1);
	parser.GetValue("ServerBindPort", &iValue2);
	parser.GetValue("ServerBindIP", &iValue3);
	parser.GetValue("WorkerThread", &iValue4);

	printf("%d \n", iValue1);
	printf("%d \n", iValue2);
	printf("%d \n", iValue3);
	printf("%d \n", iValue4);
}