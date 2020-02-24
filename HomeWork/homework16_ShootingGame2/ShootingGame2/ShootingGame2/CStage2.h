#pragma once
#include "CStageBase.h"
#include "CEnemy.h"
#include "CMessageQueue.h"
#include "CSceneManager.h"
#include "CObjectManager.h"

class CStage2 : public CStageBase
{	
	CMessageQueue* messageQueue = new CMessageQueue();
	CSceneManager* manager;
	CObjectManager* objectManager;
public:

	virtual void EnemyInit();
	virtual void Update()
	{
		objectManager->Action();
		objectManager->Draw();
	}

	CStage2() {};
	CStage2(CSceneManager* _manager)
	{
		manager = _manager;
		objectManager = new CObjectManager(manager);
		objectManager->CreateObject(manager, messageQueue, PLAYER);
		EnemyInit();
	};
	~CStage2() {
		delete objectManager;
		delete messageQueue;
	};
};

