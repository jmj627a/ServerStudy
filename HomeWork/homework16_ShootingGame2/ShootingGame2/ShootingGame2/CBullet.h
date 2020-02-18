#pragma once
#include "CBaseObject.h"
#include "global.h"

class CObjectManager;

class CBullet : public CBaseObject
{
	int orderType;
public:
	CBullet( int _x, int _y, int _orderType)
		: CBaseObject(BULLET, _x, _y, true)
	{
		orderType = _orderType;
	};
	~CBullet() {};

	virtual bool Action();
	virtual bool Draw();

	bool bullet_Move();
};

