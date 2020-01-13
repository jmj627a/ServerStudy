#include <Windows.h>
#include "Scene.h"
#include "Console.h"
#include "global.h"
#include "Bullet.h"

void Bullet::bulletInit()
{
	//free(g_Bullet);
	//g_Bullet = (BULLET*)malloc(sizeof(BULLET) * BULLET_NUM);

	//for (int i = 0; i < BULLET_NUM; ++i)
	//{
	//	g_Bullet[i].flag = false;
	//}

	flag = false;
}

void Bullet::Draw_Bullet(char chSprite)
{
	//for (int i = 0; i < BULLET_NUM; ++i)
	{
		if (true == flag)
		{
			szScreenBuffer[y][x] = chSprite;
		}
	}
}

//누구 총알인지, 몬스터면 몇번째 몬스터한테 나온 총알인지
bool Bullet::create_bullet(bool type, int num)
{
	if (false == flag)
	{
		switch (type)
		{
		case PLAYER_TYPE:
			x = g_player.x;
			y = g_player.y;
			flag = true;
			type = PLAYER_TYPE;
			break;

		case ENEMY_TYPE:
			x = g_Enemy[num].x;
			y = g_Enemy[num].y;
			flag = true;
			type = ENEMY_TYPE;
			break;
		}

		return true;
	}

	return false;
}

void Bullet::bullet_Move()
{
	if (false == flag)
		return;

	switch (type)
	{
	case PLAYER_TYPE:
		y--;
		break;

	case ENEMY_TYPE:
		y++;
		break;
	}
	
}

//총알 , 벽
bool Bullet::collision_Bullet_Wall()
{
	if (true != flag)
		return false;

	if (y < 0)
		flag = false;

	if (y > dfSCREEN_HEIGHT - 1)
		flag = false;
	
}

//총알 , 적
bool Bullet::collision_Bullet_Enemy()
{
	if (false == flag)
		return false;

	if (PLAYER_TYPE != type)
		return false;

	for (int j = 0; j < ENEMY_NUM; ++j)
	{
		if (false == g_Enemy[j].flag)
			continue;

		if ((x == g_Enemy[j].x) && (y == g_Enemy[j].y))
		{
			flag = false;
			g_Enemy[j].flag = false;
		}
	}
	
}

//총알 , 플레이어
bool Bullet::collision_Bullet_Player()
{
	if (false == flag)
		return false;

	if (ENEMY_TYPE != type)
		return false;

	if ((x == g_player.x) && (y == g_player.y))
	{
		g_player.life--;
		flag = false;
	}
}