#include "CEnemy.h"

bool CEnemy::Action()
{
	if (flag == false)
		return false;


	//�̵�
	enemy_Move();
	
	//�Ѿ˹߻�

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

//�� �̵� ���� ����
void CEnemy::enemy_changeDir()
{
	if (true != flag)
		return;

	if (abs(startX - X) > 15)
	{
		dir = (dir + 1) % 2;
	}
}

//�� ��ĭ�� �̵�
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