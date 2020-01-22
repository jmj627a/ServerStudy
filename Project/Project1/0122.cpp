#include <stdio.h>

#define hello 100
constexpr int ONE = 1;


void main()
{
	auto b = 2;
	scanf_s("%d", &b);
	const int a = b;
	int* p = (int*)&a;
	*p = 20;
	printf("%d", a);
}