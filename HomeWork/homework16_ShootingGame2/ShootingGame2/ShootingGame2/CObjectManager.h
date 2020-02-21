#pragma once
#include "CBaseObject.h"

class CMessageQueue;

class CObjectManager
{
public:
	CBaseObject* objectList[100];
	
	CObjectManager()
	{
		for (int i = 0; i < 100; ++i)
			objectList[i] = nullptr;
		//objectList = (CBaseObject*)malloc(sizeof(CBaseObject)*100);
	}

	~CObjectManager()
	{
		DestroyAllObject();
	}

	//player
	void CreateObject(CMessageQueue* _message, int _objectType);

	//bullet
	void CreateObject(int _objectType, int _X, int _Y, int _orderType);

	//enemy
	void CreateObject(int _objectType, int _X, int _Y, int _startX, int startY, bool _dir);

	void DestroyObject(int i);
	void DestroyAllObject();
	void Action();
	void Draw();
	

	
};

