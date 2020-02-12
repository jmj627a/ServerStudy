#pragma once
#include "CBaseObject.h"
#include "global.h"

class CPlayer : public CBaseObject
{
	int _life;

public:

	CPlayer() : CBaseObject(PLAYER, 40,20)
	{
		_life = 3;
	}
	~CPlayer() {};



};

