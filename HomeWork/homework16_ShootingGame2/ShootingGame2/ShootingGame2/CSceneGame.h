#pragma once
#include "CSceneBase.h"
#include "CStageBase.h"

class CSceneManager;

class CSceneGame : public CSceneBase
{
	CStageBase* stage;

public:

	CSceneGame() {};
	CSceneGame(CSceneManager* _manager, int stageType);

	~CSceneGame();
	

	virtual void Update()
	{
		stage->Update();
	}
	
};

