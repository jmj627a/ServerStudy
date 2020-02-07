#include <stdio.h>

int test(int a, int b, int c)
{
	printf("%d %d %d", a, b, c);

	return 0xaa;
}
int main()
{
	printf("%s", __TIME__);
	int a=0;
	int x = test(++a, ++a, ++a);
	return 0;
}