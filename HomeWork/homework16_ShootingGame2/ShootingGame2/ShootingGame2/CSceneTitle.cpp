#include <stdio.h>
#include <windows.h>
#include "CSceneTitle.h"
#include "CSceneManager.h"
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
		messageQueue->Enq(KEY_UP);
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		messageQueue->Enq(KEY_DOWN);
	}
	if (GetAsyncKeyState(VK_RETURN))
	{
		switch (chooseStage)
		{
		case 0:
			messageQueue->Enq(ENTER_STAGE1);
			break;
		case 1:
			messageQueue->Enq(ENTER_STAGE2);
			break;
		case 2:
			messageQueue->Enq(ENTER_STAGE3);
			break;
		}
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
				printf("¡¡");
				break;

			case 1:
				printf("¡á");
				break;
			}
		}
	}


	gotoxy(48, 5);
	printf("STAGE 1    ");

	gotoxy(48, 10);
	printf("STAGE 2    ");

	gotoxy(48, 15);
	printf("STAGE 3    ");


	gotoxy(55, 5 * (chooseStage + 1));
	printf("¢¸");
}

bool CSceneTitle::checkMessage()
{
	int message;
	int flag = messageQueue->Deq(&message);

	if (flag == false)
		return false;


	switch(message)
	{
		case KEY_UP:
			if (chooseStage > 0)
				chooseStage -= 1;
			break;

		case KEY_DOWN:
			if (chooseStage < 2)
				chooseStage += 1;
			break;

		case ENTER_STAGE1:
			manager->LoadScene(GAME_STAGE1);
			break;

		case ENTER_STAGE2:
			manager->LoadScene(GAME_STAGE2);
			break;

		case ENTER_STAGE3:
			manager->LoadScene(GAME_STAGE3);
			break;

	}

	return true;
}
