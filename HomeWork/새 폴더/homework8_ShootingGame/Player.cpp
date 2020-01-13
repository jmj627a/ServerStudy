#include "Player.h"
#include "Console.h"
#include "Scene.h"
#include "global.h"
#include <Windows.h>

void Player::playerInit()
{
	x = 40;
	y = 20;

	life = 3;
}

bool Player::isPlayerDeath()
{
	if (life < 0)
		return true;
	else
		return false;
}
void Player::playerDeathCheck()
{
	if (isPlayerDeath())
	{
		scene.Change_Scene(GAMEOVER);
	}
}

//키보드 입력
void Player::KeyInput()
{
	if (GetAsyncKeyState(VK_LEFT))
	{
		if (x > 0)
			x -= 1;
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		if (x < dfSCREEN_WIDTH - 2)
			x += 1;
	}
	if (GetAsyncKeyState(VK_UP))
	{
		if (y > 0)
			y -= 1;
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		if (y < dfSCREEN_HEIGHT - 1)
			y += 1;
	}
	if (GetAsyncKeyState(VK_SPACE))
	{
		create_bullet(PLAYER_TYPE, 0);
	}
	if (GetAsyncKeyState(VK_F12))
	{
		g_GameStage = (g_GameStage + 1) % g_TotalStage;
		scene.Change_Scene(LOADING);
	}
}

// 버퍼의 특정 위치에 원하는 문자를 출력.
void Player::Draw_Player(int iX, int iY, char chSprite)
{
	szScreenBuffer[iY][iX] = chSprite;
}