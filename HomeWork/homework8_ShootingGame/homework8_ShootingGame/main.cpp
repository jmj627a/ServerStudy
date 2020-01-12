#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <memory.h>
#include <Windows.h>
#include "Console.h"

#define ENEMY_NUM 100
#define BULLET_NUM 100

#define PLAYER_TYPE 0
#define ENEMY_TYPE 1

enum GameSceneType
{
	TITLE = 0,
	INGAME = 1,
	GAMEOVER = 2,
	LOADING = 3
};

struct PLAYER
{
	int x = 10;
	int y = 10;
};

struct ENEMY
{
	bool flag;
	
	int x;
	int y;
};


struct BULLET
{
	bool flag;
	bool type; // PLAYER_TYPE 0 , ENEMY_TYPE 1

	int x;
	int y;
};


ENEMY* g_Enemy;
BULLET g_Bullet[BULLET_NUM];
PLAYER g_player;

int g_GameSceneType = LOADING;// INGAME;
int g_GameStage = 0;
int g_TotalStage = 3;

void playerInit()
{
	g_player.x = 40;
	g_player.y = 20;
}

void enemyInit()
{
	free(g_Enemy);
	g_Enemy = (ENEMY*)malloc(sizeof(ENEMY) * ENEMY_NUM);
	
	FILE* pStageFile = fopen("Enemy.txt", "rb");

	fseek(pStageFile, 0, SEEK_END);
	int size = ftell(pStageFile);

	char* pStageBuffer = (char*)malloc(size/g_TotalStage);
	rewind(pStageFile);
	
	//fseek(pStageFile, 1889 * g_GameStage, SEEK_SET);
	//int temp = fread(pStageBuffer, 1, 1, pStageFile);
	//
	//char temp22 = *pStageBuffer;
	//printf("%c\n",temp22);

	//1886 = 80*23 + 2*23			82 * 23
	fseek(pStageFile, 3 * (g_GameStage+1) + g_GameStage * 1886, SEEK_SET);

	int temp = fread(pStageBuffer, 1, 1886, pStageFile);
	pStageBuffer[1887] = '\0';

	//printf("%s", pStageBuffer);


	int count = 0;
	for (int i = 0; i < 1886; ++i)
	{
		if (pStageBuffer[i] == '7')
		{
			g_Enemy[count].flag = true;
			g_Enemy[count].x = i % 82;
			g_Enemy[count].y = i / 82;

			count++;
		}
	}
	
	//system("pause");
}

void bulletInit()
{
	//free(g_Bullet);
	//g_Bullet = (BULLET*)malloc(sizeof(BULLET) * BULLET_NUM);

	for (int i = 0; i < BULLET_NUM; ++i)
	{
		g_Bullet[i].flag = false;
	}
}


// 화면 깜빡임을 없애기 위한 화면 버퍼.
char szScreenBuffer[dfSCREEN_HEIGHT][dfSCREEN_WIDTH];


// 버퍼의 내용을 화면으로 찍어주는 함수.
void Buffer_Flip(void);


// 화면 버퍼를 지워주는 함수
void Buffer_Clear(void);

// 버퍼의 특정 위치에 원하는 문자를 출력.
void Draw_Player(int iX, int iY, char chSprite);
void Draw_Enemy(char chSprite);
void Draw_Bullet(char chSprite);


//넘겨주는 씬으로 변경 
void Change_Scene(int type)
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
	}
}

//누구 총알인지, 몬스터면 몇번째 몬스터한테 나온 총알인지
void create_bullet(bool type, int num)
{
	for (int i = 0; i < BULLET_NUM; ++i)
	{
		if (false == g_Bullet[i].flag)
		{
			switch (type)
			{
			case PLAYER_TYPE:
				g_Bullet[i].x = g_player.x;
				g_Bullet[i].y = g_player.y;
				g_Bullet[i].flag = true;
				g_Bullet[i].type = PLAYER_TYPE;
				break;

			case ENEMY_TYPE:
				g_Bullet[i].x = g_Enemy[num].x;
				g_Bullet[i].y = g_Enemy[num].y;
				g_Bullet[i].flag = true;
				g_Bullet[i].type = ENEMY_TYPE;
				break;
			}

			return;
		}
	}
}

//키보드 입력
void KeyInput()
{
	if (GetAsyncKeyState(VK_LEFT))
	{
		if(g_player.x > 0)
			g_player.x -= 1;
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		if (g_player.x < dfSCREEN_WIDTH - 2)
			g_player.x += 1;
	}
	if (GetAsyncKeyState(VK_UP))
	{
		if (g_player.y > 0)
			g_player.y -= 1;
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		if (g_player.y < dfSCREEN_HEIGHT - 1)
			g_player.y += 1;
	}
	if (GetAsyncKeyState(VK_SPACE))
	{
		create_bullet(PLAYER_TYPE, 0);
	}
	if (GetAsyncKeyState(VK_F12))
	{
		g_GameStage = (g_GameStage + 1) % g_TotalStage;
		Change_Scene(LOADING);
	}
}

void bullet_Move()
{
	for (int i = 0; i < BULLET_NUM; ++i)
	{
		if (true == g_Bullet[i].flag)
		{
			switch (g_Bullet[i].type)
			{
			case PLAYER_TYPE:
				g_Bullet[i].y--;
				break;

			case ENEMY_TYPE:
				g_Bullet[i].y++;
				break;
			}
		}
	}
}

//총알 , 벽
void collision_Bullet_Wall()
{
	for (int i = 0; i < BULLET_NUM; ++i)
	{
		if (true == g_Bullet[i].flag)
		{
			if (g_Bullet[i].y < 0)
				g_Bullet[i].flag = false;

			if (g_Bullet[i].y >= dfSCREEN_HEIGHT)
				g_Bullet[i].flag = false;
		}
	}
}

//게임씬
void Game_Scene()
{
	//Render
	// 스크린 버퍼를 지움
	Buffer_Clear();

	// 스크린 버퍼에 스프라이트 출력 
	Draw_Player(g_player.x, g_player.y, 'A');
	Draw_Enemy('M');
	Draw_Bullet('O');

	// 스크린 버퍼를 화면으로 출력
	Buffer_Flip();

	//키입력
	KeyInput();

	//로직처리
	//enemy_move();
	bullet_Move();

	collision_Bullet_Wall();
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

			break;

		case INGAME:
			Game_Scene();
			break;

		case GAMEOVER:

			break;

		case LOADING:
			playerInit();
			enemyInit();
			bulletInit();
			Change_Scene(INGAME);
			break;
		}


		// 프레임 맞추기용 대기 10 Frame
		Sleep(50);
	}
}



// 버퍼의 내용을 화면으로 찍어주는 함수.
void Buffer_Flip(void)
{
	//cs_ClearScreen();

	for (int i = 0; i < dfSCREEN_HEIGHT; ++i)
	{
		cs_MoveCursor(0, i);
		printf("%s\n", szScreenBuffer[i]);
	}

	
	//그냥 창 테두리
	for (int i = 0; i < dfSCREEN_HEIGHT; ++i)
	{
		cs_MoveCursor(dfSCREEN_WIDTH - 2, i);
		printf("│");
	}

	cs_MoveCursor(0, dfSCREEN_HEIGHT);
	printf("─────────────────────────────────────────────────────────────────────────────────");
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

//--------------------------------------------------------------------
// 버퍼의 특정 위치에 원하는 문자를 출력.
//
// 입력 받은 X,Y 좌표에 아스키코드 하나를 출력한다. (버퍼에 그림)
//--------------------------------------------------------------------
void Draw_Player(int iX, int iY, char chSprite)
{
	szScreenBuffer[iY][iX] = chSprite;
}

void Draw_Enemy(char chSprite)
{
	for (int i = 0; i < ENEMY_NUM; ++i)
	{
		if(true == g_Enemy[i].flag)
			szScreenBuffer[g_Enemy[i].y][g_Enemy[i].x] = chSprite;
	}
}


void Draw_Bullet(char chSprite)
{
	for (int i = 0; i < BULLET_NUM; ++i)
	{
		if (true == g_Bullet[i].flag)
		{
			szScreenBuffer[g_Bullet[i].y][g_Bullet[i].x] = chSprite;
		}
	}
}