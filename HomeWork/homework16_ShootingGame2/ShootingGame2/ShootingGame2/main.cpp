#include <stdio.h>
#include <Windows.h>
#include "CObjectManager.h"
#include "CSceneManager.h"
#include "CMessageQueue.h"
#include "global.h"

void main()
{
	cs_Initial();
	Buffer_Clear();

	CSceneManager* sceneManager = new CSceneManager();

	while (1)
	{
		Buffer_Flip();
		Buffer_Clear();

		sceneManager->Action();

		Sleep(50);
	} 
}