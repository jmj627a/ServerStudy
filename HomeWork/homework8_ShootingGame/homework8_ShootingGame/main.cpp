#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <memory.h>
#include <Windows.h>
#include "Console.h"

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
	int x;
	int y;
};

//struct STAGE_FILE
//{
//	unsigned int stage;
//	unsigned int data
//};

ENEMY g_Enemy[100];
BULLET g_Bullet[100];
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

	printf("%s", pStageBuffer);


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
	
	system("pause");
}

//--------------------------------------------------------------------
// 화면 깜빡임을 없애기 위한 화면 버퍼.
//--------------------------------------------------------------------
char szScreenBuffer[dfSCREEN_HEIGHT][dfSCREEN_WIDTH];


//--------------------------------------------------------------------
// 버퍼의 내용을 화면으로 찍어주는 함수.
// 1 프레임이 끝나는 마지막에 본 함수를 호출하여 버퍼 -> 화면 으로 그린다.
//--------------------------------------------------------------------
void Buffer_Flip(void);


//--------------------------------------------------------------------
// 화면 버퍼를 지워주는 함수
//--------------------------------------------------------------------
void Buffer_Clear(void);

//--------------------------------------------------------------------
// 버퍼의 특정 위치에 원하는 문자를 출력.
//--------------------------------------------------------------------
void Sprite_Draw(int iX, int iY, char chSprite);



//키보드 입력
void KeyInput()
{
	if (GetAsyncKeyState(VK_LEFT))
	{
		g_player.x -= 1;
	}
	else if (GetAsyncKeyState(VK_RIGHT))
	{
		g_player.x += 1;
	}
	else if (GetAsyncKeyState(VK_UP))
	{
		g_player.y -= 1;
	}
	else if (GetAsyncKeyState(VK_DOWN))
	{
		g_player.y += 1;
	}
}


void Game_Scene()
{
	//Render
	// 스크린 버퍼를 지움
	Buffer_Clear();

	// 스크린 버퍼에 스프라이트 출력 
	Sprite_Draw(g_player.x, g_player.y, 'A');

	// 스크린 버퍼를 화면으로 출력
	Buffer_Flip();

	//키입력
	KeyInput();

	//로직처리
}


void main(void)
{

	cs_Initial();
	Buffer_Clear();
	//--------------------------------------------------------------------
	// 게임의 메인 루프
	// 이 루프가  1번 돌면 1프레임 이다.
	//--------------------------------------------------------------------
	
	while (1)
	{
		switch (g_GameSceneType)
		{
		case TITLE:

			break;

		case INGAME:

			break;

		case GAMEOVER:

			break;

		case LOADING:
			playerInit();
			enemyInit();
			break;
		}


		// 프레임 맞추기용 대기 10 Frame
		Sleep(50);
	}
}



//--------------------------------------------------------------------
// 버퍼의 내용을 화면으로 찍어주는 함수.
//
// 적군,아군,총알 등을 szScreenBuffer 에 넣어주고, 
// 1 프레임이 끝나는 마지막에 본 함수를 호출하여 버퍼 -> 화면 으로 그린다.
//--------------------------------------------------------------------
void Buffer_Flip(void)
{
	//cs_ClearScreen();

	for (int i = 0; i < dfSCREEN_HEIGHT; ++i)
	{
		cs_MoveCursor(0, i);
		printf("%s", szScreenBuffer[i]);
	}
}


//--------------------------------------------------------------------
// 화면 버퍼를 지워주는 함수
//
// 매 프레임 그림을 그리기 직전에 버퍼를 지워 준다. 
// 안그러면 이전 프레임의 잔상이 남으니까
//--------------------------------------------------------------------
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
void Sprite_Draw(int iX, int iY, char chSprite)
{
	szScreenBuffer[iY][iX] = chSprite;

}




