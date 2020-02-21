#pragma once
#include "CBaseObject.h"
#include "global.h"

class CObjectManager;

class CBullet : public CBaseObject
{
	int orderType;
	CObjectManager* objectManager;

public:
	CBullet(CObjectManager* _objectManager, int _x, int _y, int _orderType)
		: CBaseObject(BULLET, _x, _y, true)
	{
		objectManager = _objectManager;
		orderType = _orderType;
	};
	~CBullet() {};

	virtual bool Action();
	virtual bool Draw();

	bool bullet_Move();

	//충돌체크
	void collision_Bullet_Wall();
	void collision_Bullet_Enemy();
	void collision_Bullet_Player();
};

