#include <stdio.h>
#include "CParse.h"

void main()
{
	CParse parser;
	int iValue1, iValue2;

	parser.LoadFile("info.txt");
	parser.GetValue("Version", &iValue1);
	parser.GetValue("ServerID", &iValue2);


}