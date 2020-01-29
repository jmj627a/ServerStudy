#include <stdio.h>

constexpr int ONE = 1;
constexpr int TWO = 2;

#define ONEONE 1
#define TWOTWO 2

void main()
{
	int a = ONE + TWO;

	int b = ONEONE + TWOTWO;

	printf("%d \n", a + b);

}