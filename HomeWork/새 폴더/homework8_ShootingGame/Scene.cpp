#include <Windows.h>
#include "Scene.h"
#include "Console.h"
#include "global.h"
#include "stdio.h"

int temp = 0;
void Scene:: Gameover_Scene()
{
	cs_ClearScreen();

	cs_MoveCursor(28, 10);
	printf("　GO TO THE TITLE");

	cs_MoveCursor(28, 15);
	printf("　EXIT");

	cs_MoveCursor(28, 10 + 5 * (temp));
	printf("▶");

	if (GetAsyncKeyState(VK_UP))
	{
		if (temp > 0)
			temp -= 1;
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		if (temp < 1)
			temp += 1;
	}
	if (GetAsyncKeyState(VK_RETURN))
	{
		switch (temp)
		{
		case 0:
			Change_Scene(TITLE);
			break;
		case 1:
			Change_Scene(EXIT);
			break;
		}
	}
	cs_MoveCursor(28, 10 + 5 * (temp));
	printf("▶");
}

//넘겨주는 씬으로 변경 
void Scene::Change_Scene(int type)
{
	switch (type)
	{
	case TITLE:
		g_GameSceneType = TITLE;	break;

	case INGAME:
		g_GameSceneType = INGAME;	break;

	case GAMEOVER:
		g_GameSceneType = GAMEOVER;		break;

	case LOADING:
		g_GameSceneType = LOADING;		break;

	case EXIT:
		g_GameSceneType = EXIT;		break;
	}
}

//타이틀 씬
void Scene::Title_Scene()
{
	Title_Print();

	Title_keyInput();

}

void Scene::Title_Print()
{
	cs_ClearScreen();

	for (int i = 0; i < 20; ++i)
	{
		cs_MoveCursor(0, i);
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


	cs_MoveCursor(48, 5);
	printf("　STAGE 1");

	cs_MoveCursor(48, 10);
	printf("　STAGE 2");

	cs_MoveCursor(48, 15);
	printf("　STAGE 3");

	cs_MoveCursor(48, 20);
	printf("　EXIT");

	cs_MoveCursor(48, 5 * (g_GameStage + 1));
	printf("▶");
}

void Scene::Title_keyInput()
{
	if (GetAsyncKeyState(VK_UP))
	{
		if (g_GameStage > 0)
			g_GameStage -= 1;

		cs_MoveCursor(48, 5 * (g_GameStage + 1));
		printf("▶");
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		if (g_GameStage < g_TotalStage - 1)
			g_GameStage += 1;


		cs_MoveCursor(48, 5 * (g_GameStage + 1));
		printf("▶");
	}
	if (GetAsyncKeyState(VK_RETURN))
	{
		Change_Scene(LOADING);
	}
}

//게임씬
void Scene::Game_Scene()
{
	//Render
	// 스크린 버퍼를 지움
	Buffer_Clear();

	// 스크린 버퍼에 스프라이트 출력 
	g_player.Draw_Player(g_player.x, g_player.y, 'A');
	Draw_Enemy('M');

	for(int i=0; i<BULLET_NUM; ++i)
		g_Bullet[i].Draw_Bullet('O');

	// 스크린 버퍼를 화면으로 출력
	Buffer_Flip();
	Draw_interface();

	//키입력
	g_player.KeyInput();

	//로직처리

	enemy_Move();
	
	for (int i = 0; i < BULLET_NUM; ++i)
		g_Bullet[i].bullet_Move();

	enemy_Attack();

	for (int i = 0; i < BULLET_NUM; ++i)
		g_Bullet[i].collision_Bullet_Wall();
	
	for (int i = 0; i < BULLET_NUM; ++i)
		g_Bullet[i].collision_Bullet_Enemy();
	
	for (int i = 0; i < BULLET_NUM; ++i)
		g_Bullet[i].collision_Bullet_Player();

	g_player.playerDeathCheck();

}