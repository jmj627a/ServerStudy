#include "CBullet.h"
#include "CObjectManager.h"
#include "CPlayer.h"

bool CBullet::Action()
{
	if (true != flag)
		return false;

	//이동
	bullet_Move();

	//충돌체크
	collision_Bullet_Wall();
	collision_Bullet_Enemy();
	collision_Bullet_Player();

	return true;
}

bool CBullet::Draw()
{
	if (false == flag)
		return false;

	szScreenBuffer[Y][X] = 'O';

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

void CBullet::collision_Bullet_Wall()
{
	if (true != flag)
		return;

	if (Y < 0)
		flag = false;

	if (Y > dfSCREEN_HEIGHT - 1)
		flag = false;
}

void CBullet::collision_Bullet_Enemy()
{
	if (true != flag)
		return;

	if (ENEMY == orderType)
		return;

	for (int i = 0; i < ENEMY_NUM; ++i)
	{
		if (false == flag)
			continue;

		if ((X == objectManager->objectList[i]->getX()) && (Y == objectManager->objectList[i]->getY()))
		{
			flag = false;
			objectManager->objectList[i]->SetFlag(false);
		}
	}
}

void CBullet::collision_Bullet_Player()
{
	if (true != flag)
		return;

	if (PLAYER == orderType)
		return;

	if ((X == objectManager->objectList[0]->getX()) && (Y == objectManager->objectList[0]->getY()))
	{
		CPlayer* player = (CPlayer*)(objectManager->objectList[0]);
		player->setLife(-1);
		flag = false;
	}
}
