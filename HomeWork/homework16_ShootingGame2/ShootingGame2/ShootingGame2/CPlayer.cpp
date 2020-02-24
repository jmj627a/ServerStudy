#include <Windows.h>
#include "CPlayer.h"
#include "CMessageQueue.h"
#include "CObjectManager.h"

bool CPlayer::Action()
{
	if (true != flag) 
		return false;

	//Ű�Է�
	player_KeyInput();

	//�޼���ť ������ �̵�
	while (checkMessage());

	playerDeathCheck();

	ui->printUI();

	return true;
}

// ������ Ư�� ��ġ�� ���ϴ� ���ڸ� ���.
bool CPlayer::Draw()
{
	if (true != flag) 
		return false;

	szScreenBuffer[Y][X] = 'A';
	return true;
}


//�÷��̾� ü���� 0 �������� Ȯ��
bool CPlayer::isPlayerDeath()
{
	if (life < 0)
		return true;
	else
		return false;
}

void CPlayer::playerDeathCheck()
{
	if (isPlayerDeath())
	{
		message->Enq(GAME_OVER);
	}
}

//Ű���� �Է�
void CPlayer::player_KeyInput()
{
	if (GetAsyncKeyState(VK_LEFT))
	{
		message->Enq(KEY_LEFT);
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		message->Enq(KEY_RIGHT);
	}
	if (GetAsyncKeyState(VK_UP))
	{
		message->Enq(KEY_UP);
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		message->Enq(KEY_DOWN);
	}
	if (GetAsyncKeyState(VK_SPACE))
	{
		message->Enq(KEY_SPACE);
	}

	
}

bool CPlayer::checkMessage()
{
	int _message;
	bool isMessage= message->Deq(&_message);

	if (isMessage == false)
		return false;

	//����Ű �̵���, �Ѿ� �߻翡 ���� �޼��� �з�
	switch (_message)
	{
	case KEY_UP:
		if(Y > 1)
			Y--;
		break;

	case KEY_DOWN:
		if (Y < dfSCREEN_HEIGHT - 1)
			Y++;
		break;

	case KEY_RIGHT:
		if (X < dfSCREEN_WIDTH - 2)
			X++;
		break;

	case KEY_LEFT:
		if (X > 1)
			X--;
		break;

	case KEY_SPACE:
		//�Ѿ˻���
		objectManager->CreateObject(BULLET, X, Y, PLAYER);
		break;

	case GAME_OVER:
		//���ӿ���
		ui->deleteUI();
		manager->LoadScene(GAMEOVER);
		break;
	}

	return true;
}