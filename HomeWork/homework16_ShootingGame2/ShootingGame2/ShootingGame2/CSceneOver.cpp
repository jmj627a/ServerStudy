#include <Windows.h>
#include "CSceneOver.h"
#include "CSceneManager.h"
#include "global.h"

void CSceneOver::Update()
{
	Over_keyInput();

	while (checkMessage());

	Over_Print();
}

void CSceneOver::Over_Print()
{
	cs_ClearScreen();

	gotoxy(28, 10);
	printf("¡¡GO TO THE TITLE");

	gotoxy(28, 15);
	printf("¡¡EXIT");

	gotoxy(28, 10 + 5 * (chooseMenu));
	printf("¢º");
}

void CSceneOver::Over_keyInput()
{
	if (GetAsyncKeyState(VK_UP))
	{
		messageQueue->Enq(KEY_UP);
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		messageQueue->Enq(KEY_DOWN);
	}
	if (GetAsyncKeyState(VK_RETURN))
	{
		messageQueue->Enq(KEY_ENTER);
	}
}

bool CSceneOver::checkMessage()
{
	int message;
	int flag = messageQueue->Deq(&message);

	if (flag == false)
		return false;


	switch (message)
	{
	case KEY_UP:
		if (chooseMenu > 0)
			chooseMenu -= 1;
		break;

	case KEY_DOWN:
		if (chooseMenu < 1)
			chooseMenu += 1;
		break;

	case KEY_ENTER:
		switch (chooseMenu)
		{
		case 0:
			manager->LoadScene(TITLE);
			cs_ClearScreen();
			break;
		case 1:
			manager->LoadScene(EXIT);
			cs_ClearScreen();
			break;
		}
	}

	return true;
}