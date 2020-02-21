#include "CEnemy.h"
#include "CObjectManager.h"

bool CEnemy::Action()
{
	if (flag == false)
		return false;

	//이동
	enemy_Move();
	
	//총알발사
	enemy_Attack();

	return true;
}

bool CEnemy::Draw()
{
	if (true == flag)
		szScreenBuffer[Y][X] = 'M';

	return true;
}

//적 이동 방향 변경
void CEnemy::enemy_changeDir()
{
	if (true != flag)
		return;

	if (abs(startX - X) > 15)
	{
		dir = (dir + 1) % 2;
	}
}

//적 한칸씩 이동
void CEnemy::enemy_Move()
{
	enemy_changeDir();

	if (true != flag)
		return;

	switch (dir)
	{
	case false:
		X++;
		break;
	case true:
		X--;
		break;
	}
}


//랜덤하게 총알 발사
void CEnemy::enemy_Attack()
{
	if (false == flag)
		return;

	int random = rand() % 100;

	if (random <= 5) //그냥 하나당 5%의 확률로 총알 발사
	{
		objectManager->CreateObject(BULLET, X, Y, ENEMY);
	}
}