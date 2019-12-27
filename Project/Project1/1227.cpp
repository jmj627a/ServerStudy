#include <stdio.h>

#pragma pack(1)
struct Data
{
	int a;
	char b;
	__declspec(align(64)) short c;
	char d;
	int e;
	__int64 f;
	char g;
};
#pragma pack()

void Test(Data* d)
{
	d->a = 1;
	d->b = 1;
	d->c = 1;
}
void main()
{
	printf("%d", sizeof(Data));
	Data d;
	Test(&d);
}