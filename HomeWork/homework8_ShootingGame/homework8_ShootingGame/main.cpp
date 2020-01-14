#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <memory.h>
#include <Windows.h>
#include <stdlib.h>
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
	LOADING = 3,
	EXIT = 4
};

struct Player
{
	int x = 10;
	int y = 10;

	int life = 3;
};

struct Enemy
{
	bool flag = false;
	
	int x;
	int y;

	int startX;
	int startY;
	bool dir; //0 - ������ , 1 - ����
};

struct Bullet
{
	bool flag = false;
	bool type; // 0 - PLAYER_TYPE , 1 - ENEMY_TYPE

	int x;
	int y;
};

struct Title
{
	int text[6][3];

	int startX;
	int startY;

	int x;
	int y;

	bool dir;
	int size = 6;
};


Enemy* g_Enemy;
Bullet g_Bullet[BULLET_NUM];
Player g_player;

int g_GameSceneType = TITLE;
int g_GameStage = 0;
int g_TotalStage = 3;

bool Special_state = false;

// ȭ�� �������� ���ֱ� ���� ȭ�� ����.
char szScreenBuffer[dfSCREEN_HEIGHT][dfSCREEN_WIDTH];

//Ÿ��Ʋ ����
char szTitleBuffer[20][20] = {0,};

Title title[4] = {
	{{0,1,0,
	 1,0,0,
	 1,1,1,
	 1,0,1,
	 1,0,1,
	 0,1,0}, 2,2,2,2,0,6},
	{{0,1,0,
	 1,0,1,
	 1,1,1,
	 1,0,1,
	 1,0,1,
	 0,0,0}, 6,5,6,5,0,6},
	{{1,0,1,
	 1,1,1,
	 1,1,1,
	 1,0,1,
	 1,0,1,
	 0,0,0}, 10,9,10,9,0,6},
	{{1,1,1,
	 1,0,0,
	 1,1,1,
	 1,0,0,
	 1,1,1,
	 0,0,0}, 14,12,14,12,0,6}
};

void playerInit();
void enemyInit();
void bulletInit();


// ������ ������ ȭ������ ����ִ� �Լ�.
void Buffer_Flip(void);

// ȭ�� ���۸� �����ִ� �Լ�
void Buffer_Clear(void);

//Ÿ��Ʋ ���� ���
void Title_Print();

// ������ Ư�� ��ġ�� ���ϴ� ���ڸ� ���.
void Draw_Player(int iX, int iY, char chSprite);
void Draw_Enemy(char chSprite);
void Draw_Bullet(char chSprite);

//���ۿ� ��°� �ƴϰ� �׳� ���
void Draw_interface();

void Draw_Title();

//�Ѿ� ���� (���� �Ѿ�����, ���͸� ���° �������� ���� �Ѿ�����)
void create_bullet(bool type, int num);

//Ű���� �Է�
void KeyInput();
void Title_keyInput();

//�� ĭ�� �̵�
void bullet_Move();
void enemy_Move();

void Title_Move();

//�� �̵� ���� ����
void enemy_changeDir();

//�����ϰ� �Ѿ� �߻�
void enemy_Attack();

//�浹üũ
void collision_Bullet_Wall();
void collision_Bullet_Enemy();
void collision_Bullet_Player();


//�÷��̾� ü���� 0 �������� Ȯ��
bool isPlayerDeath();
void playerDeathCheck();


int temp = 0;

//�Ѱ��ִ� ������ ���� 
void Change_Scene(int type);

//�� �Լ�
void Title_Scene();
void Game_Scene();
void Gameover_Scene();

//�� �� ������� ���� ����������
void stageEndCheck();


void main(void)
{
	cs_Initial();
	Buffer_Clear();
	
	while (1)
	{
		switch (g_GameSceneType)
		{
		case TITLE:
			Title_Scene();
			break;

		case INGAME:
			Game_Scene();
			break;

		case GAMEOVER:
			Gameover_Scene();
			break;

		case LOADING:
			//clear();
			playerInit();
			enemyInit();
			bulletInit();
			Change_Scene(INGAME);
			cs_ClearScreen();
			break;

		case EXIT:
			return;
		}


		// ������ ���߱�� ��� 10 Frame
		Sleep(50);
	}
}

void playerInit()
{
	g_player.x = 40;
	g_player.y = 20;

	g_player.life = 3;
}

void enemyInit()
{
	free(g_Enemy);
	g_Enemy = (Enemy*)malloc(sizeof(Enemy) * ENEMY_NUM);

	FILE* pStageFile = fopen("Enemy.txt", "rb");

	fseek(pStageFile, 0, SEEK_END);
	int size = ftell(pStageFile);

	char* pStageBuffer = (char*)malloc(size / g_TotalStage);
	rewind(pStageFile);

	//fseek(pStageFile, 1889 * g_GameStage, SEEK_SET);
	//int temp = fread(pStageBuffer, 1, 1, pStageFile);
	//
	//char temp22 = *pStageBuffer;
	//printf("%c\n",temp22);

	//1886 = 80*23 + 2*23			82 * 23
	fseek(pStageFile, 3 * (g_GameStage + 1) + g_GameStage * 1886, SEEK_SET);

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

			g_Enemy[count].startX = g_Enemy[count].x;
			g_Enemy[count].startY = g_Enemy[count].y;
			g_Enemy[count].dir = 0;

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


// ������ ������ ȭ������ ����ִ� �Լ�.
void Buffer_Flip(void)
{
	//cs_ClearScreen();

	for (int i = 0; i < dfSCREEN_HEIGHT; ++i)
	{
		cs_MoveCursor(0, i);
		printf("%s\n", szScreenBuffer[i]);
	}
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

// ������ Ư�� ��ġ�� ���ϴ� ���ڸ� ���.
void Draw_Player(int iX, int iY, char chSprite)
{
	szScreenBuffer[iY][iX] = chSprite;
}

void Draw_Enemy(char chSprite)
{
	for (int i = 0; i < ENEMY_NUM; ++i)
	{
		if (true == g_Enemy[i].flag)
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

	case EXIT:
		g_GameSceneType = EXIT;		break;
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
		if (g_player.x > 0)
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
	if (GetAsyncKeyState(VK_F1))
	{
		Special_state = (Special_state + 1) % 2;
	}
}

//�Ѿ� ��ĭ�� �̵�
void bullet_Move()
{
	for (int i = 0; i < BULLET_NUM; ++i)
	{
		if (true != g_Bullet[i].flag)
			continue;

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

//�� �̵� ���� ����
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

//�� ��ĭ�� �̵�
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

//�����ϰ� �Ѿ� �߻�
void enemy_Attack()
{
	int random = rand() % (ENEMY_NUM);

	if (true == g_Enemy[random].flag)
	{
		create_bullet(ENEMY_TYPE, random);
	}
}

//�Ѿ� , ��
void collision_Bullet_Wall()
{
	for (int i = 0; i < BULLET_NUM; ++i)
	{
		if (true != g_Bullet[i].flag)
			continue;

		if (g_Bullet[i].y < 0)
			g_Bullet[i].flag = false;

		if (g_Bullet[i].y > dfSCREEN_HEIGHT - 1)
			g_Bullet[i].flag = false;
	}
}

//�Ѿ� , ��
void collision_Bullet_Enemy()
{
	for (int i = 0; i < BULLET_NUM; ++i)
	{
		if (false == g_Bullet[i].flag)
			continue;

		if (PLAYER_TYPE != g_Bullet[i].type)
			continue;

		for (int j = 0; j < ENEMY_NUM; ++j)
		{
			if (false == g_Enemy[j].flag)
				continue;

			if ((g_Bullet[i].x == g_Enemy[j].x) && (g_Bullet[i].y == g_Enemy[j].y))
			{
				g_Bullet[i].flag = false;
				g_Enemy[j].flag = false;
			}
		}
	}
}

//�Ѿ� , �÷��̾�
void collision_Bullet_Player()
{
	for (int i = 0; i < BULLET_NUM; ++i)
	{
		if (false == g_Bullet[i].flag)
			continue;

		if (ENEMY_TYPE != g_Bullet[i].type)
			continue;

		if ((g_Bullet[i].x == g_player.x) && (g_Bullet[i].y == g_player.y))
		{
			g_player.life--;
			g_Bullet[i].flag = false;
		}
	}
}

//���ۿ� ��°� �ƴϰ� �׳� ���
void Draw_interface()
{
	//â �׵θ�
	for (int i = 0; i < dfSCREEN_HEIGHT; ++i)
	{
		cs_MoveCursor(dfSCREEN_WIDTH - 1, i);
		printf("��");
	}

	cs_MoveCursor(0, dfSCREEN_HEIGHT);
	printf("������������������������������������������������������������������������������������������������������������������������������������������������������������������");

	//���
	cs_MoveCursor(0, dfSCREEN_HEIGHT + 1);
	printf(" life : ������");

	cs_MoveCursor(0, dfSCREEN_HEIGHT + 1);
	printf(" life : ");

	for (int i = 0; i < g_player.life; ++i)
		printf("��");
}

//�÷��̾� ü���� 0 �������� Ȯ��
bool isPlayerDeath()
{
	if (g_player.life < 0)
		return true;
	else
		return false;
}

void playerDeathCheck()
{
	if (isPlayerDeath())
	{
		Change_Scene(GAMEOVER);
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
	Draw_interface();

	//Ű�Է�
	KeyInput();

	//����ó��
	enemy_Move();
	bullet_Move();
	enemy_Attack();

	collision_Bullet_Wall();
	collision_Bullet_Enemy();
	collision_Bullet_Player();

	playerDeathCheck();
	stageEndCheck();

}

//Ÿ��Ʋ �̵�
void Title_Move()
{
	for (int i = 0; i < 4; ++i)
	{
		switch (title[i].dir)
		{
		case 0:
			if (title[i].y < 15)
				title[i].y++;
			else
				title[i].dir = (title[i].dir + 1) % 2;
			break;

		case 1:
			if (title[i].y > 0)
				title[i].y--;
			else
				title[i].dir = (title[i].dir + 1) % 2;
			break;
		}
		
	}
}

//Ÿ��Ʋ ���
void Draw_Title()
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
				//cs_MoveCursor(title[i].startX + j, title[i].startY + k);
				szTitleBuffer[title[i].y + k][title[i].x + j] = title[i].text[k][j];
			}
		}
	}
}

//Ÿ��Ʋ ���� ���
void Title_Print()
{

	for (int i = 0; i < 20; ++i)
	{
		cs_MoveCursor(0, i);
		for (int j = 0; j < 20; ++j)
		{
			switch (szTitleBuffer[i][j])
			{
			case 0:
				printf("��");
				break;

			case 1:
				printf("��");
				break;
			}
		}
	}


	cs_MoveCursor(48, 5);
	printf("��STAGE 1");

	cs_MoveCursor(48, 10);
	printf("��STAGE 2");

	cs_MoveCursor(48, 15);
	printf("��STAGE 3");


	cs_MoveCursor(48, 5 * (g_GameStage + 1));
	printf("��");
}

//Ÿ��Ʋ Ű�Է�
void Title_keyInput()
{
	if (GetAsyncKeyState(VK_UP))
	{
		if (g_GameStage > 0)
			g_GameStage -= 1;

		cs_MoveCursor(48, 5 * (g_GameStage + 1));
		printf("��");
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		if (g_GameStage < g_TotalStage - 1)
			g_GameStage += 1;


		cs_MoveCursor(48, 5 * (g_GameStage + 1));
		printf("��");
	}
	if (GetAsyncKeyState(VK_RETURN))
	{
		Change_Scene(LOADING);
	}
}

//Ÿ��Ʋ �� �Լ�
void Title_Scene()
{
	Title_Move();

	Draw_Title();

	Title_Print();

	Title_keyInput();
}

//���ӿ��� �� �Լ�
void Gameover_Scene()
{
	cs_ClearScreen();

	cs_MoveCursor(28, 10);
	printf("��GO TO THE TITLE");

	cs_MoveCursor(28, 15);
	printf("��EXIT");

	cs_MoveCursor(28, 10 + 5 * (temp));
	printf("��");

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
			cs_ClearScreen();
			break;
		case 1:
			Change_Scene(EXIT);
			cs_ClearScreen();
			break;
		}
	}
	cs_MoveCursor(28, 10 + 5 * (temp));
	printf("��");
}

//���� �� ������ ���� ����������
void stageEndCheck()
{
	bool flag = true;
	for (int i = 0; i < ENEMY_NUM; ++i)
	{
		if (true == g_Enemy[i].flag)
		{
			flag = false;
		}
	}

	if (true == flag)
	{
		g_GameStage = (g_GameStage + 1) % g_TotalStage;
		Change_Scene(LOADING);
	}
}