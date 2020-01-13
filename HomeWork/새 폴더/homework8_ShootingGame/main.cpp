#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <memory.h>
#include <Windows.h>
#include <stdlib.h>
#include "Console.h"
#include "Player.h"
#include "global.h"
#include "Scene.h"
#include "Enemy.h"


// 버퍼의 내용을 화면으로 찍어주는 함수.
void Buffer_Flip(void)
{
	//cs_ClearScreen();

	for (int i = 0; i < dfSCREEN_HEIGHT; ++i)
	{
		cs_MoveCursor(0, i);
		printf("%s\n", szScreenBuffer[i]);
	}
}


// 화면 버퍼를 지워주는 함수
void Buffer_Clear(void)
{
	for (int i = 0; i < dfSCREEN_HEIGHT; ++i)
	{
		for (int j = 0; j < dfSCREEN_WIDTH - 1; ++j)
		{
			szScreenBuffer[i][j] = ' ';
		}
		szScreenBuffer[i][dfSCREEN_WIDTH - 1] = '\0';
	}
}



void Draw_Enemy(char chSprite)
{
	for (int i = 0; i < ENEMY_NUM; ++i)
	{
		if (true == g_Enemy[i].flag)
			szScreenBuffer[g_Enemy[i].y][g_Enemy[i].x] = chSprite;
	}
}


void enemy_changeDir()
{
	for (int i = 0; i < ENEMY_NUM; ++i)
	{
		if (true != g_Enemy[i].flag)
			continue;

		if (abs(g_Enemy[i].startX - g_Enemy[i].x) > 10)
		{
			g_Enemy[i].dir = (g_Enemy[i].dir + 1) % 2;
		}
	}
}

void enemy_Move()
{
	enemy_changeDir();

	for (int i = 0; i < ENEMY_NUM; ++i)
	{
		if (true != g_Enemy[i].flag)
			continue;
	
		switch (g_Enemy[i].dir)
		{
		case false: 
			g_Enemy[i].x++;
			break;
		case true:
			g_Enemy[i].x--;
			break;
		}
	}
}

void enemy_Attack()
{
	int random = rand() % (ENEMY_NUM);

	if (true == g_Enemy[random].flag)
	{
		for (int i = 0; i < BULLET_NUM; ++i)
			if (g_Bullet[i].create_bullet(ENEMY_TYPE, random))
				break;
	}
}



//버퍼에 찍는건 아니고 그냥 출력
void Draw_interface()
{
	//창 테두리
	for (int i = 0; i < dfSCREEN_HEIGHT; ++i)
	{
		cs_MoveCursor(dfSCREEN_WIDTH - 1, i);
		printf("│");
	}

	cs_MoveCursor(0, dfSCREEN_HEIGHT);
	printf("────────────────────────────────────────────────────────────────────────────────┘");

	//목숨
	cs_MoveCursor(0, dfSCREEN_HEIGHT + 1);
	printf(" life : ♡♡♡");

	cs_MoveCursor(0, dfSCREEN_HEIGHT+1);
	printf(" life : ");

	for (int i = 0; i < g_player.life; ++i)
		printf("♥");
}




void main(void)
{
	cs_Initial();
	Buffer_Clear();
	
	while (1)
	{
		switch (g_GameSceneType)
		{
		case TITLE:
			scene.Title_Scene();
			break;

		case INGAME:
			scene.Game_Scene();
			break;

		case GAMEOVER:
			scene.Gameover_Scene();
			break;

		case LOADING:
			//clear();
			g_player.playerInit();
			enemyInit();
			for (int i = 0; i < BULLET_NUM; ++i)
				g_Bullet[i].bulletInit();
			scene.Change_Scene(INGAME);
			break;

		case EXIT:
			return;
		}


		// 프레임 맞추기용 대기 10 Frame
		Sleep(50);
	}
}

