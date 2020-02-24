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
	virtual bool Draw();	// 버퍼의 특정 위치에 원하는 문자를 출력.

	//플레이어 체력이 0 이하인지 확인
	bool isPlayerDeath();
	void playerDeathCheck();
	void setLife(int _life) { 
		life += _life; 
	}
	int getLife() {
		return life;
	}

	//키보드 입력
	void player_KeyInput();

	//메세지 큐에서 뽑아서 메세지 확인
	bool checkMessage();

};

