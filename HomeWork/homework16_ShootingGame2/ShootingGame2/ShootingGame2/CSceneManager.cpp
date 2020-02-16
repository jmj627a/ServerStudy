#include "CSceneManager.h"

void CSceneManager::Action()
{
	_pScene->Update();
}


void CSceneManager::LoadScene(int SceneType)
{
	switch (SceneType) 
	{
	case 0: //title scene
		_pScene = new CSceneTitle(this);
		cs_ClearScreen();
		break;
	case 1:
	case 2:
	case 3:
		_pScene = new CSceneGame(this, SceneType);
		cs_ClearScreen();
		break;
	}
}
