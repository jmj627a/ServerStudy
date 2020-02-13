#pragma once
#include "CSceneBase.h"
#include "CSceneTitle.h"
#include "global.h"

class CSceneManager
{
public:
	CSceneBase* _pScene;

	CSceneManager()
	{
		LoadScene(TITLE);
	}

	void Action();

	void LoadScene(int SceneType);
};

