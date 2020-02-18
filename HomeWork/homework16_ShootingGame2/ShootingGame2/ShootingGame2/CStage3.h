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

		////키보드
		//Title_keyInput();
		//
		////메세지큐 꺼내오기
		//while (checkMessage());
		//
		////로직
		//Title_Move();
		//
		////랜더
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

