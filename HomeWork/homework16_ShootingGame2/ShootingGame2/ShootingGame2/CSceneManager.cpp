#include "CSceneManager.h"

void CSceneManager::Action()
{
	_pScene->Update();
}


void CSceneManager::LoadScene(int SceneType)
{
	switch (SceneType) 
	{
	case TITLE: //title scene
		_pScene = new CSceneTitle(this);
		cs_ClearScreen();
		break;
	case GAME_STAGE1:
	case GAME_STAGE2:
	case GAME_STAGE3:
		_pScene = new CSceneGame(this, SceneType);
		cs_ClearScreen();
		break;
	case GAMEOVER:
		_pScene = new CSceneOver(this);
		cs_ClearScreen();
		break;
	case EXIT:
		cs_ClearScreen();
		delete this;
		return;
	}
}
