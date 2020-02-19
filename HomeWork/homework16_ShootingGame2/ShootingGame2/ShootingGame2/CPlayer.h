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
	virtual bool Draw();	// ������ Ư�� ��ġ�� ���ϴ� ���ڸ� ���.

	//�÷��̾� ü���� 0 �������� Ȯ��
	bool isPlayerDeath();
	void playerDeathCheck();

	//Ű���� �Է�
	void player_KeyInput();
};

