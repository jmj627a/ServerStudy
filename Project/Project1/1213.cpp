#include <stdio.h>

void Test(int a)
{
	a = 10;
}

int main() 
{
	int x;
	x = 0;
	Test(x);
	x = 1;
	Test(x);
	x = 2;

	return 0;
}