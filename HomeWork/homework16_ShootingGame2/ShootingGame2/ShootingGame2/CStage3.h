#pragma once
#include "CStageBase.h"
#include "CEnemy.h"
#include "CMessageQueue.h"
#include "CSceneManager.h"
#include "CObjectManager.h"
#include "UI.h"

class CStage3 : public CStageBase
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

	CStage3() {};
	CStage3(CSceneManager* _manager)
	{
		manager = _manager;
		objectManager = new CObjectManager(manager);
		objectManager->CreateObject(manager, messageQueue, PLAYER);
		EnemyInit();
	};
	~CStage3() {
		delete objectManager;
		delete messageQueue;
	};
};

