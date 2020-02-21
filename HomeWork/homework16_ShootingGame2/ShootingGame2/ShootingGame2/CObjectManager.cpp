#include "CObjectManager.h"
#include "global.h"
#include "CBullet.h"
#include "CPlayer.h"
#include "CEnemy.h"

void CObjectManager::CreateObject(CMessageQueue* _message, int _objectType)
{
	for (int i = 0; i < 100; ++i)
	{
		if (objectList[i] != nullptr)
			continue;

		objectList[i] = new CPlayer(this, _message);
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
	for (int i = 0; i < 100; ++i)
	{
		delete objectList[i];
		objectList[i] = nullptr;
	}
}

void CObjectManager::Action()
{
	for (int i = 0; i < 100; ++i)
	{
		if (objectList[i] == nullptr)
			continue;

		objectList[i]->Action();
	}
	
	DestroyObject();
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



