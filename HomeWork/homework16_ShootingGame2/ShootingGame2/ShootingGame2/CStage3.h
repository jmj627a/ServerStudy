#pragma once
#include "CStageBase.h"
#include "CEnemy.h"
#include "CMessageQueue.h"
#include "CSceneManager.h"
#include "CObjectManager.h"
class CStage3 : public CStageBase
{
	//CPlayer* player;
	CEnemy* enemy[100];
	CMessageQueue* messageQueue = new CMessageQueue();
	CSceneManager* manager;// = new CSceneManager();
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
		////�޼���ť ��������
		//while (checkMessage());
		//
		////����
		//Title_Move();
		//
		////����
		//Title_Draw();
		//Title_Print();

	}
	CStage3() {};
	CStage3(CSceneManager* _manager) {
		manager = _manager;
		EnemyInit();
	};
	~CStage3() {
		delete manager;
		delete messageQueue;
		for (int i = 0; i < 100; ++i)
			delete enemy[i];
	};
};

