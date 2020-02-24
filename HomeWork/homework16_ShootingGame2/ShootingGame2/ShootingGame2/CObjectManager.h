#pragma once
#include "CBaseObject.h"

class CMessageQueue;
class CSceneManager;

class CObjectManager
{
public:
	CBaseObject* objectList[100];
	CSceneManager* manager;

	CObjectManager(CSceneManager* _manager)
	{
		manager = _manager;
		for (int i = 0; i < 100; ++i)
			objectList[i] = nullptr;
	}

	~CObjectManager()
	{
		DestroyAllObject();
	}

	//player
	void CreateObject(CSceneManager* _manager, CMessageQueue* _message, int _objectType);

	//bullet
	void CreateObject(int _objectType, int _X, int _Y, int _orderType);

	//enemy
	void CreateObject(int _objectType, int _X, int _Y, int _startX, int startY, bool _dir);
	void stageEndCheck(); //list에 enemy가 없으면 타이틀로

	void DestroyObject();
	void DestroyAllObject();
	void Action();
	void Draw();
	

	
};

