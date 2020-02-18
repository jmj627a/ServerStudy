#pragma once
#include "CBaseObject.h"
#include "global.h"

class CObjectManager;

class CEnemy : public CBaseObject
{
	int startX;
	int startY;
	bool dir;
	CObjectManager* objectManager;

public:
	CEnemy(CObjectManager* _objectManager, int _x, int _y, int _startX, int _startY, bool _dir)
		: CBaseObject(ENEMY, _x, _y, true) 
	{
		startX = _startX;
		startY = _startY;
		dir = _dir;
		objectManager = _objectManager;
	};

	~CEnemy() {};


	virtual bool Action();
	virtual bool Draw();

	//적 이동 방향 변경
	void enemy_changeDir();

	//적 한칸씩 이동
	void enemy_Move();
	
	//랜덤하게 총알 발사
	void enemy_Attack();

};

