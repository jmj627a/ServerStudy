#pragma once
#include "CSceneBase.h"
#include "CStageBase.h"
#include "global.h"

class CSceneManager;

class CSceneGame : public CSceneBase
{
	CStageBase* stage;

public:

	CSceneGame() {};
	CSceneGame(CSceneManager* _manager, int stageType);

	~CSceneGame() {
		delete stage;
	}
	

	virtual void Update()
	{
		stage->Update();
	}
	virtual int getSceneType()
	{
		return GAME_STAGE1;
	}
	
};

