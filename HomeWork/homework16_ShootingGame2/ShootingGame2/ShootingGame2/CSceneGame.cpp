#include "CSceneGame.h"
#include "CSceneManager.h"
#include "CStage1.h"
#include "CStage2.h"
#include "CStage3.h"

CSceneGame::CSceneGame(CSceneManager * _manager, int stageType)
{
	switch (stageType)
	{
	case 1:
		stage = new CStage1(_manager); break;
	case 2:
		stage = new CStage2(_manager); break;
	case 3:
		stage = new CStage3(_manager); break;
	}
}
