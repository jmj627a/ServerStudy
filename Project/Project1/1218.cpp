#include <stdio.h>

//�Լ� �����͸� ���ڷ� �޴� �Լ�
void Func(int x, int y, void(*callback)(int error))
{
	//x y�� �̿��� ���� ����

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