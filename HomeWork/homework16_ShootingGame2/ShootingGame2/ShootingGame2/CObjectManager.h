#pragma once
#include "CBaseObject.h"
#include <list>
#include <iostream>

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
		DestroyObject();
	}

	void CreateObject(int _objectType, int _X, int _Y, int _orderType);
	
	void DestroyObject();
	void Action();
	void Draw();
	

	
};

