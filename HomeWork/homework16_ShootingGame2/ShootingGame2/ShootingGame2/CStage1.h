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
	CObjectManager* objectManager;
public:

	virtual void EnemyInit();
	virtual void Update()
	{
		objectManager->Action();
		objectManager->Draw();
	}

	CStage1() {};
	CStage1(CSceneManager* _manager)
	{
		manager = _manager;
		objectManager = new CObjectManager(manager);
		objectManager->CreateObject(manager, messageQueue, PLAYER);
		EnemyInit();
	};
	~CStage1() {
		delete objectManager;
		delete messageQueue;
	};
};

