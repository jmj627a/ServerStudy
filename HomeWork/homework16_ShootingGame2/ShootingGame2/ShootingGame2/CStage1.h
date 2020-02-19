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
		

		////Ű����
		//Title_keyInput();
		//
		//�޼���ť ��������
		while (checkMessage());
		//
		////����
		//Title_Move();
		//
		////����
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

