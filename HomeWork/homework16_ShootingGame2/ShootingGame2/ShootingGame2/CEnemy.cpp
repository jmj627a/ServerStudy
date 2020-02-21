#include "CEnemy.h"
#include "CObjectManager.h"

bool CEnemy::Action()
{
	if (flag == false)
		return false;

	//�̵�
	enemy_Move();
	
	//�Ѿ˹߻�
	enemy_Attack();

	return true;
}

bool CEnemy::Draw()
{
	if (true == flag)
		szScreenBuffer[Y][X] = 'M';

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


//�����ϰ� �Ѿ� �߻�
void CEnemy::enemy_Attack()
{
	if (false == flag)
		return;

	int random = rand() % 100;

	if (random <= 5) //�׳� �ϳ��� 5%�� Ȯ���� �Ѿ� �߻�
	{
		objectManager->CreateObject(BULLET, X, Y, ENEMY);
	}
}