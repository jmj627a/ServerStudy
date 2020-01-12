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


// ȭ�� �������� ���ֱ� ���� ȭ�� ����.
char szScreenBuffer[dfSCREEN_HEIGHT][dfSCREEN_WIDTH];


// ������ ������ ȭ������ ����ִ� �Լ�.
void Buffer_Flip(void);


// ȭ�� ���۸� �����ִ� �Լ�
void Buffer_Clear(void);

// ������ Ư�� ��ġ�� ���ϴ� ���ڸ� ���.
void Draw_Player(int iX, int iY, char chSprite);
void Draw_Enemy(char chSprite);
void Draw_Bullet(char chSprite);


//�Ѱ��ִ� ������ ���� 
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

//���� �Ѿ�����, ���͸� ���° �������� ���� �Ѿ�����
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

//Ű���� �Է�
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

//�Ѿ� , ��
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

//���Ӿ�
void Game_Scene()
{
	//Render
	// ��ũ�� ���۸� ����
	Buffer_Clear();

	// ��ũ�� ���ۿ� ��������Ʈ ��� 
	Draw_Player(g_player.x, g_player.y, 'A');
	Draw_Enemy('M');
	Draw_Bullet('O');

	// ��ũ�� ���۸� ȭ������ ���
	Buffer_Flip();

	//Ű�Է�
	KeyInput();

	//����ó��
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


		// ������ ���߱�� ��� 10 Frame
		Sleep(50);
	}
}



// ������ ������ ȭ������ ����ִ� �Լ�.
void Buffer_Flip(void)
{
	//cs_ClearScreen();

	for (int i = 0; i < dfSCREEN_HEIGHT; ++i)
	{
		cs_MoveCursor(0, i);
		printf("%s\n", szScreenBuffer[i]);
	}

	
	//�׳� â �׵θ�
	for (int i = 0; i < dfSCREEN_HEIGHT; ++i)
	{
		cs_MoveCursor(dfSCREEN_WIDTH - 2, i);
		printf("��");
	}

	cs_MoveCursor(0, dfSCREEN_HEIGHT);
	printf("������������������������������������������������������������������������������������������������������������������������������������������������������������������");
}


// ȭ�� ���۸� �����ִ� �Լ�
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
// ������ Ư�� ��ġ�� ���ϴ� ���ڸ� ���.
//
// �Է� ���� X,Y ��ǥ�� �ƽ�Ű�ڵ� �ϳ��� ����Ѵ�. (���ۿ� �׸�)
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