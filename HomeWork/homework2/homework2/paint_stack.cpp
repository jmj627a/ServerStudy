#include <stdio.h>
#include <Windows.h>

#define SCREEN_WIDTH 10
#define SCREEN_HEIGHT 10

#define BLANK 0
#define BEFORE 1
#define AFTER 2

class NODE
{
public:
	int xPos;
	int yPos;

	NODE* next;

	NODE() {}
	~NODE() {}

	NODE(int _xPos, int _yPos){
		xPos = _xPos;
		yPos = _yPos;
		next = NULL;
	}
};
class STACK
{
public:
	NODE *top = new NODE(2, 1);

	int pop(NODE& result)
	{
		if (NULL == top)
			return false; // false

		result = *top;

		NODE* ptr = top;
		top = top->next;

		delete ptr;
		return true;
	}

	void push(int _xPos, int _yPos)
	{
		NODE* e = new NODE{ _xPos, _yPos };
		e->next = top;
		top = e;
	}

	void clear()
	{
		while (top != NULL)
		{
			NODE* temp = top;
			top = top->next;
			delete temp;
		}

		push(2, 1);
	}
};


STACK paintStack;
NODE temp;

void boardPrint(int board[][SCREEN_WIDTH])
{
	for (int i = 0; i < SCREEN_HEIGHT; ++i)
	{
		for (int j = 0; j < SCREEN_WIDTH; ++j)
		{
			switch (board[i][j])
			{
			case BLANK:
				printf("¡¡");
				break;
			case BEFORE:
				printf("¢½");
				break;
			case AFTER:
				printf("¢¾");
				break;
			}
		}
		printf("\n");
	}
}

void paintBoard(int board[][SCREEN_WIDTH])
{

	system("cls");
	boardPrint(board);
	Sleep(10);

	if (temp.xPos < 0 || temp.xPos >= SCREEN_WIDTH)
		return;
	else if (temp.yPos < 0 || temp.yPos >= SCREEN_HEIGHT)
		return;


	if (board[temp.xPos][temp.yPos] == BEFORE)
		board[temp.xPos][temp.yPos] = AFTER;
	else if (board[temp.xPos][temp.yPos] == AFTER)
	{
		 return;
	}
	else if (board[temp.xPos][temp.yPos] == BLANK)
		return;

	paintStack.push(temp.xPos, temp.yPos - 1);
	paintStack.push(temp.xPos, temp.yPos + 1);
	paintStack.push(temp.xPos - 1, temp.yPos);
	paintStack.push(temp.xPos + 1, temp.yPos);

}

void main()
{
	while (true)
	{
		paintStack.clear();

		int board[SCREEN_WIDTH][SCREEN_HEIGHT] = {
		{0, 1,1,0,0,0,1,1,0,0,},
		{1, 1,1,1,0,1,1,1,1,0,},
		{1, 1,1,1,1,1,1,1,1,0,},
		{1, 1,0,0,1,0,0,1,1,0,},
		{1, 1,0,0,0,0,0,1,1,0,},
		{1, 1,1,0,0,0,1,1,1,0,},
		{0, 1,1,1,0,1,1,1,0,0,},
		{0, 0,1,1,1,1,1,0,0,1,},
		{0, 0,0,1,1,1,0,0,1,1,},
		{0, 0,0,0,1,0,0,1,1,1,}
		};

		while (true == paintStack.pop(temp))
		{
			paintBoard(board);
		}
	}
}