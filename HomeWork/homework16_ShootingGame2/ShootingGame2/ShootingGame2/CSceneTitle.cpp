#include <stdio.h>
#include <windows.h>
#include "CSceneTitle.h"
#include "global.h"

void CSceneTitle::Title_Move()
{
	for (int i = 0; i < 4; ++i)
	{
		switch (word[i].dir)
		{
		case 0:
			if (word[i].y < 15)
				word[i].y++;
			else
				word[i].dir = (word[i].dir + 1) % 2;
			break;

		case 1:
			if (word[i].y > 0)
				word[i].y--;
			else
				word[i].dir = (word[i].dir + 1) % 2;
			break;
		}

	}
}

void CSceneTitle::Title_keyInput()
{
	if (GetAsyncKeyState(VK_UP))
	{
		if (chooseStage > 0)
			chooseStage -= 1;

		//cs_MoveCursor(48, 5 * (chooseStage + 1));
		//printf("▶");
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		if (chooseStage < 3)
			chooseStage += 1;


		//cs_MoveCursor(48, 5 * (chooseStage + 1));
		//printf("▶");
	}
	if (GetAsyncKeyState(VK_RETURN))
	{
		//chooseStage를 manager에게 알려주면서 return??
		//아니면 keyinput이니까 큐에 넣으면서 그냥 나감
		//LoadScene(LOADING);
	}
}

void CSceneTitle::Title_Draw()
{
	for (int i = 0; i < 20; ++i)
	{
		for (int j = 0; j < 20; ++j)
		{
			szTitleBuffer[j][i] = 0;
		}
	}

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			for (int k = 0; k < 6; ++k)
			{
				szTitleBuffer[word[i].y + k][word[i].x + j] = word[i].text[k][j];
			}
		}
	}
}


void CSceneTitle::Title_Print()
{
	for (int i = 0; i < 20; ++i)
	{
		gotoxy(0, i);
		for (int j = 0; j < 20; ++j)
		{
			switch (szTitleBuffer[i][j])
			{
			case 0:
				printf("　");
				break;

			case 1:
				printf("■");
				break;
			}
		}
	}


	gotoxy(48, 5);
	printf("   STAGE 1");

	gotoxy(48, 10);
	printf("   STAGE 2");

	gotoxy(48, 15);
	printf("   STAGE 3");


	cs_MoveCursor(48, 5 * (chooseStage + 1));
	printf("▶");
}
