#include "CObjectManager.h"
#include "global.h"
#include "CBullet.h"
#include "CPlayer.h"
#include "CEnemy.h"

void CObjectManager::CreateObject(CSceneManager* _manager, CMessageQueue* _message, int _objectType)
{
	for (int i = 0; i < 100; ++i)
	{
		if (objectList[i] != nullptr)
			continue;

		objectList[i] = new CPlayer(this, _manager, _message);
		return;
	}
}

void CObjectManager::CreateObject(int _objectType, int _X, int _Y, int _orderType)
{
	for (int i = 0; i < 100; ++i)
	{
		if (objectList[i] != nullptr)
			continue;

		if (_orderType == ENEMY)
			objectList[i] = new CBullet(this, _X, _Y, ENEMY);
		else if (_orderType == PLAYER)
			objectList[i] = new CBullet(this, _X, _Y, PLAYER);
		return;
	}

}

void CObjectManager::CreateObject(int _objectType, int _X, int _Y, int _startX, int _startY, bool _dir)
{
	for (int i = 0; i < 100; ++i)
	{
		if (objectList[i] != nullptr)
			continue;

		objectList[i] = new CEnemy(this, _X, _Y, _startX, _startY, 0);
		return;
	}
}

void CObjectManager::stageEndCheck()
{
	bool flag = true;
	for (int i = 0; i < 100; ++i)
	{
		if (objectList[i] == nullptr)
			continue;

		if (ENEMY == objectList[i]->GetObjectType())
		{
			flag = false;
			break;
		}
	}

	if (true == flag)
	{
		cs_ClearScreen();
		manager->LoadScene(TITLE);
	}
	
}

void CObjectManager::DestroyObject()
{
	for (int i = 0; i < 100; ++i)
	{
		if (objectList[i] == nullptr)
			continue;

		if (objectList[i]->GetFlag() == false)
		{
			delete objectList[i];
			objectList[i] = nullptr;
		}
	}
}

void CObjectManager::DestroyAllObject()
{
	delete[] objectList;
}

void CObjectManager::Action()
{
	for (int i = 0; i < 100; ++i)
	{
		if (objectList[i] == nullptr)
			continue;

		objectList[i]->Action();
		stageEndCheck();

	}
	
	DestroyObject();

	//if ((TITLE != manager->getSceneType()) &&
	//	(GAMEOVER != manager->getSceneType()))
		//stageEndCheck();
}

void CObjectManager::Draw()
{
	for (int i = 0; i < 100; ++i)
	{
		if (objectList[i] == nullptr)
			continue;

		objectList[i]->Draw();
	}
}



