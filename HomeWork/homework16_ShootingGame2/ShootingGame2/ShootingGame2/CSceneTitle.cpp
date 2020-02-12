#include <stdio.h>
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
		cs_MoveCursor(0, i);
		for (int j = 0; j < 20; ++j)
		{
			switch (szTitleBuffer[i][j])
			{
			case 0:
				printf("¡¡");
				break;

			case 1:
				printf("¡á");
				break;
			}
		}
	}


	cs_MoveCursor(48, 5);
	printf("¡¡STAGE 1");

	cs_MoveCursor(48, 10);
	printf("¡¡STAGE 2");

	cs_MoveCursor(48, 15);
	printf("¡¡STAGE 3");


	cs_MoveCursor(48, 5 * (g_GameStage + 1));
	printf("¢º");
}
