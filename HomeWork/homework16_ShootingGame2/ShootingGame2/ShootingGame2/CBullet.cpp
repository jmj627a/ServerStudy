#include "CBullet.h"


bool CBullet::Action()
{
	if (true != flag)
		return false;

	//¿Ãµø
	bullet_Move();

	return true;
}

bool CBullet::Draw()
{
	for (int i = 0; i < ENEMY_NUM; ++i)
	{
		if (true == flag)
			szScreenBuffer[Y][X] = 'O';
	}

	return true;
}

bool CBullet::bullet_Move()
{
	if (true != flag)
		return false;

	switch (orderType)
	{
	case PLAYER:
		Y--;
		break;

	case ENEMY:
		Y++;
		break;
	}
}
