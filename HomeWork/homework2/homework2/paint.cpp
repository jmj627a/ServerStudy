#include <stdio.h>
#include <Windows.h>

#define SCREEN_WIDTH 10
#define SCREEN_HEIGHT 10

#define BLANK 0
#define BEFORE 1
#define AFTER 2

bool key = false;


void boardPrint(int board[][SCREEN_WIDTH])
{
	for (int i = 0; i < SCREEN_HEIGHT; ++i)
	{
		for (int j = 0; j < SCREEN_WIDTH; ++j)
		{
			switch (board[i][j])
			{
			case BLANK:
				printf("��");
				break;
			case BEFORE:
				printf("��");
				break;
			case AFTER:
				printf("��");
				break;
			}
		}
		printf("\n");
	}
}

void paintBoard(int x, int y, int board[][SCREEN_WIDTH])
{

	system("cls");
	boardPrint(board);
	Sleep(10);

	if (x < 0 || x >= SCREEN_WIDTH)
		return;
	else if (y < 0 || y >= SCREEN_HEIGHT)
		return;


	if (board[x][y] == BEFORE)
		board[x][y] = AFTER;
	else if (board[x][y] == AFTER)
	{
		if (x == 2 && y == 1) {}
		else return;
	}
	else if (board[x][y] == BLANK)
		return;

	paintBoard(x - 1, y, board);
	paintBoard(x, y - 1, board);
	paintBoard(x + 1, y, board);
	paintBoard(x, y + 1, board);

	if (x == 2 && y == 1)
		key = true;
}

void main()
{
	while (true)
	{
		key = false;

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

		while (false == key)
		{
			paintBoard(2, 1, board);
		}
	}
}