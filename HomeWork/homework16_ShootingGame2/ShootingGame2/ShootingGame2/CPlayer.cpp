#include <Windows.h>
#include "CPlayer.h"
#include "CMessageQueue.h"

bool CPlayer::Action()
{
	if (true != flag) return false;

	//�浹üũ


	//Ű�Է�
	player_KeyInput();

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
	if (GetAsyncKeyState(VK_F12))
	{
		message->Enq(KEY_UP);
	}
	
}