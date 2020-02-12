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
		_pScene = new CSceneTitle();
	}



	void Run();

	void LoadScene(int SceneType);
};

