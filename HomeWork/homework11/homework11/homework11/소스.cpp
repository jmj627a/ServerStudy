#include <stdio.h>
#include <Windows.h>

constexpr int OBJECT_MAX = 30;
constexpr int WINDOW_WIDTH = 90;


class CBaseObject
{
protected:
	int _X = 0;

public:
	virtual bool Action() = 0;	// 리턴으로 파괴여부 결정
	virtual void Draw(int index) = 0;
};

void gotoxy(int x, int y) {

	COORD Cur;

	Cur.X = x;

	Cur.Y = y;

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Cur);

}

class COneStar : public CBaseObject
{
public:
	bool Action()
	{
		_X += 1;

		if (_X >= WINDOW_WIDTH)
			return false;

		return true;
	}

	void Draw(int index)
	{
		gotoxy(0, index);
		for (int i = 0; i < _X; ++i)
			printf(" ");
		printf("*");
	}
};

class CTwoStar : public CBaseObject
{
public:
	bool Action()
	{
		_X += 2;

		if (_X >= WINDOW_WIDTH)
			return false;

		return true;
	}

	void Draw(int index)
	{
		gotoxy(0, index);
		for (int i = 0; i < _X; ++i)
			printf(" ");
		printf("**");
	}
};

class CThreeStar : public CBaseObject
{
public:
	bool Action()
	{
		_X += 3;

		if (_X >= WINDOW_WIDTH)
			return false;

		return true;
	}

	void Draw(int index)
	{
		gotoxy(0, index);
		for (int i = 0; i < _X; ++i)
			printf(" ");
		printf("***");
	}
};

int g_Count = 0;
CBaseObject* objectList[OBJECT_MAX];

void KeyProcess()
{
	if (GetAsyncKeyState('1'))
	{
		for (int i = 0; i < OBJECT_MAX; ++i)
		{
			if (objectList[i] == NULL)
			{
				objectList[i] = new COneStar();
				break;
			}
		}
	}
	else if (GetAsyncKeyState('2'))
	{
		for (int i = 0; i < OBJECT_MAX; ++i)
		{
			if (objectList[i] == NULL)
			{
				objectList[i] = new CTwoStar();
				break;
			}
		}
	}
	else if (GetAsyncKeyState('3'))
	{
		for (int i = 0; i < OBJECT_MAX; ++i)
		{
			if (objectList[i] == NULL)
			{
				objectList[i] = new CThreeStar();
				break;
			}
		}
	}
}

void Action()
{
	for (int i = 0; i < OBJECT_MAX; ++i)
	{
		//if (objectList[i] != NULL)
		//	continue;
		if (objectList[i] != NULL)
		{
			if (!objectList[i]->Action())
			{
				delete objectList[i];
				objectList[i] = NULL;
			}
		}
	}
}

void Draw()
{
	for (int i = 0; i < OBJECT_MAX; ++i)
	{
		if (objectList[i] != NULL)
			objectList[i]->Draw(i);
	}
}



void main()
{
	while (1)
	{
		KeyProcess();

		Action();

		system("cls");
		Draw();

		Sleep(30);
	}
}