#pragma once
#include "CSceneBase.h"
#include "CSceneTitle.h"
#include "CSceneGame.h"
#include "CSceneOver.h"
#include "global.h"

class CSceneManager
{
	CSceneBase* _pScene;
public:

	CSceneManager()
	{
		_pScene = new CSceneTitle(this);
		LoadScene(TITLE);
	}
	~CSceneManager() {
		delete _pScene;
	}

	void Action();
	void LoadScene(int SceneType);
	int getSceneType() {
		return _pScene->getSceneType();
	}

};

