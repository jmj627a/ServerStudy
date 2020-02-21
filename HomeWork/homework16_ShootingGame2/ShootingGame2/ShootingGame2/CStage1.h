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
		
		objectManager->Action();
		objectManager->Draw();
		

		//메세지큐 꺼내오기
		//플레이어 action에서 하자..
		//while (checkMessage());
	}


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

