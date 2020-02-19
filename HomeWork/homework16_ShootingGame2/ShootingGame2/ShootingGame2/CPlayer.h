#pragma once
#include "CBaseObject.h"
#include "global.h"

class CMessageQueue;

class CPlayer : public CBaseObject
{
	int life;
	CMessageQueue* message;
public:

	CPlayer(CMessageQueue* _message) : CBaseObject(PLAYER, 40,20,true)
	{
		message = _message;
		life = 3;
	}
	~CPlayer() {};

	virtual bool Action();
	virtual bool Draw();	// 버퍼의 특정 위치에 원하는 문자를 출력.

	//플레이어 체력이 0 이하인지 확인
	bool isPlayerDeath();
	void playerDeathCheck();

	//키보드 입력
	void player_KeyInput();
};

