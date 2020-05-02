#include <stdio.h>
#include "CPacket.h"
#include <crtdbg.h>


void main()
{
	CPacket* packet = new CPacket(1000);

	int a = 10;
	char b = 'c';
	short c= 65535;
	const char* str = "str";

	*packet << a;
	*packet << b;
	*packet << c;


	int aa;
	char bb;
	short cc;

	*packet >> aa;
	*packet >> bb;
	*packet >> cc;

	printf("%d %c %d", aa, bb, cc);





	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
}