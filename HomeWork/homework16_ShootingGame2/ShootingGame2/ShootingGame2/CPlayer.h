#pragma once
#include "CBaseObject.h"
#include "global.h"

class CMessageQueue;
class CObjectManager;

class CPlayer : public CBaseObject
{
	int life;
	CMessageQueue* message;
	CObjectManager* objectManager;
public:

	CPlayer(CObjectManager* _objectManager, CMessageQueue* _message) : CBaseObject(PLAYER, 40,20,true)
	{
		message = _message;
		objectManager = _objectManager;
		life = 3;
	}
	~CPlayer() {};

	virtual bool Action();
	virtual bool Draw();	// ������ Ư�� ��ġ�� ���ϴ� ���ڸ� ���.

	//�÷��̾� ü���� 0 �������� Ȯ��
	bool isPlayerDeath();
	void playerDeathCheck();
	void setLife(int _life) { 
		life += _life; 
		//if(life <= 0)
			//���ӿ���
	}

	//Ű���� �Է�
	void player_KeyInput();

	//�޼��� ť���� �̾Ƽ� �޼��� Ȯ��
	bool checkMessage();

};

