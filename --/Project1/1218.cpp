#include <stdio.h>

//함수 포인터를 인자로 받는 함수
void Func(int x, int y, void(*callback)(int error))
{
	//x y를 이용한 로직 수행

	int error = 10;
	callback(error);
}

void MoveCallBack(int error)
{
	Func(20, 10, MoveCallBack);
}

enum ENUM_TYPE
{
	TYPE_1 = 1,
	TYPE_2,
	TYPE_3
};
void main()
{

}