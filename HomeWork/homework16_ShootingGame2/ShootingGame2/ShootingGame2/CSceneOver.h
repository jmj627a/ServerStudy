#pragma once
#include "CSceneBase.h"
#include "CMessageQueue.h"

class CSceneManager;

class CSceneOver : public CSceneBase
{
	CMessageQueue* messageQueue = new CMessageQueue();
	CSceneManager* manager;
	int chooseMenu = 0;
public:
	CSceneOver() {}
	CSceneOver(CSceneManager* _manager) : manager(_manager){}
	~CSceneOver()
	{
		delete messageQueue;
	}

	virtual void Update();
	virtual int getSceneType()
	{
		return GAMEOVER;
	}
	void Over_Print();
	void Over_keyInput();
	
	bool checkMessage();
};

