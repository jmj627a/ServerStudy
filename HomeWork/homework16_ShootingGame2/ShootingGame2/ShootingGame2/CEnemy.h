#pragma once
#include "CBaseObject.h"
#include "global.h"

class CEnemy : public CBaseObject
{
	int startX;
	int startY;
	bool dir;

public:
	CEnemy(int _x, int _y, int _startX, int _startY, bool _dir) 
		: CBaseObject(ENEMY, _x, _y, true) 
	{
		startX = _startX;
		startY = _startY;
		dir = _dir;
	};
	~CEnemy() {};


	virtual bool Action();
	virtual bool Draw();

	//�� �̵� ���� ����
	void enemy_changeDir();

	//�� ��ĭ�� �̵�
	void enemy_Move();
	

};

