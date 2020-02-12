#include <stdio.h>
#include "CObjectManager.h"

void main()
{
	CObjectManager* objectManager = new CObjectManager();

	while (1)
	{
		objectManager->Action();
		objectManager->Draw();
		objectManager->Filp();
	}
}