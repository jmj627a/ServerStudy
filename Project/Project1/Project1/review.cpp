#include <stdio.h>
#include <stdlib.h>

int strlen(const char *str)
{
	int count = 0;
	while (true)
	{
		if (str[count] == '\0')
			return count;

		count++;
	}
}

void main()
{
	printf("%d \n", strlen("hello"));

}



//
//class Cone
//{
//	int a;
//	int b;
//	int c;
//
//public:
//	//Cone(int _a, int _b, int _c) : a(_a), b(_b), c(_c) {};
//	Cone() {}
//	Cone(const int& _a, const int& _b, const int& _c)
//	{
//		a = _a;
//		b = _b;
//		c = _c;
//	}
//
//	Cone& operator=(const Cone& _cone)
//	{
//		a = _cone.a;
//		b = _cone.b;
//		c = _cone.c;
//
//		return *this;
//	}
//
//	Cone& operator+(const Cone& _cone)
//	{
//
//		a += _cone.a;
//		b += _cone.b;
//		c += _cone.c;
//
//		return *this;
//	}
//};
//void main()
//{
//	Cone o(1, 2, 3);
//	Cone a = o;
//	Cone b;
//	b = o;
//	Cone B = a + o;
//
//}