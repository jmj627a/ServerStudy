#pragma once
#include "CBaseObject.h"
#include "CSceneManager.h"
#include "global.h"
#include "UI.h"

class CMessageQueue;
class CObjectManager;

class CPlayer : public CBaseObject
{
	int life;
	CMessageQueue* message;
	CObjectManager* objectManager;
	CSceneManager* manager;
	UI* ui;
public:

	CPlayer(CObjectManager* _objectManager, CSceneManager* _manager, CMessageQueue* _message) : CBaseObject(PLAYER, 40,20,true)
	{
		message = _message;
		manager = _manager;
		objectManager = _objectManager;
		life = 3;
		ui = new UI(this);
	}
	~CPlayer() {
		delete ui;
	};

	virtual bool Action();
	virtual bool Draw();	// ������ Ư�� ��ġ�� ���ϴ� ���ڸ� ���.

	//�÷��̾� ü���� 0 �������� Ȯ��
	bool isPlayerDeath();
	void playerDeathCheck();
	void setLife(int _life) { 
		life += _life; 
	}
	int getLife() {
		return life;
	}

	//Ű���� �Է�
	void player_KeyInput();

	//�޼��� ť���� �̾Ƽ� �޼��� Ȯ��
	bool checkMessage();

};

