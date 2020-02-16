#include "CObjectManager.h"
#include "global.h"

void CObjectManager::CreateObject(int ObjectType)
{
}

void CObjectManager::DestroyObject()
{
	for (int i = 0; i < 100; ++i)
		free(objectList[i]);
}

void CObjectManager::Action()
{
	for (int i = 0; i < 100; ++i)
	{
		if (objectList[i] == nullptr)
			return;

		objectList[i]->Action();
	}
}

void CObjectManager::Draw()
{
	for (int i = 0; i < 100; ++i)
	{
		if (objectList[i] == nullptr)
			return;
		objectList[i]->Draw();
	}
}



