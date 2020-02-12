#pragma once
#include "CBaseObject.h"
#include <list>
#include <iostream>

class CObjectManager
{
public:
	std::list<CBaseObject*> ObjectList;
	
	void CreateObject(int ObjectType);
	void DestroyObject();
	void Action();
	void Draw();
	void Filp();

};

