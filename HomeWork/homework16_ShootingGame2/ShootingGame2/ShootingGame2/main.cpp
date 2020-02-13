#include <stdio.h>
#include <Windows.h>
#include "CObjectManager.h"
#include "CSceneManager.h"
#include "global.h"

void main()
{
	cs_Initial();
	Buffer_Clear();

	CSceneManager* sceneManager = new CSceneManager();
	CObjectManager* objectManager = new CObjectManager();

	while (1)
	{
		objectManager->Draw();

		objectManager->Action();
		sceneManager->Action();
		//Buffer_Flip();


		Sleep(50);
	}
}