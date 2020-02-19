#pragma once
#include "CStageBase.h"
#include "CEnemy.h"
#include "CMessageQueue.h"
#include "CSceneManager.h"
#include "CObjectManager.h"

class CStage1 : public CStageBase
{
	CMessageQueue* messageQueue = new CMessageQueue();
	CSceneManager* manager;
	CObjectManager* objectManager = new CObjectManager();

public:


	virtual void EnemyInit();

	virtual void Update()
	{
		
		objectManager->Draw();
		objectManager->Action();
		

		////키보드
		//Title_keyInput();
		//
		//메세지큐 꺼내오기
		while (checkMessage());
		//
		////로직
		//Title_Move();
		//
		////랜더
		//Title_Draw();
		//Title_Print();

	}

	bool checkMessage();

	CStage1() {};
	CStage1(CSceneManager* _manager)
	{
		manager = _manager;
		objectManager->CreateObject(messageQueue, PLAYER);
		EnemyInit();
	};
	~CStage1() {
		delete manager;
		delete messageQueue;
	};
};

