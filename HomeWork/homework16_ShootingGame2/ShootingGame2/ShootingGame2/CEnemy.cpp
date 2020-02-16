#include "CEnemy.h"

bool CEnemy::Action()
{
	if (flag == false)
		return false;


	//이동
	enemy_Move();
	
	//총알발사

	return true;
}

bool CEnemy::Draw()
{
	for (int i = 0; i < ENEMY_NUM; ++i)
	{
		if (true == flag)
			szScreenBuffer[Y][X] = 'M';
	}

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