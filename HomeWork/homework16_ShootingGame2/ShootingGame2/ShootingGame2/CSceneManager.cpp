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
		_pScene = new CSceneTitle();
		break;
	}
}
