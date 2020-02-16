#pragma once
#include "CSceneBase.h"

class CSceneManager;
class CStageBase;

class CSceneGame : public CSceneBase
{
	CStageBase* stage;

public:

	CSceneGame() {};
	CSceneGame(CSceneManager* _manager, int stageType);

	~CSceneGame();
	

	virtual void Update();
	
};

