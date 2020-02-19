#include <Windows.h>
#include "CPlayer.h"
#include "CMessageQueue.h"

bool CPlayer::Action()
{
	if (true != flag) return false;

	//충돌체크


	//키입력
	player_KeyInput();

	return true;
}

// 버퍼의 특정 위치에 원하는 문자를 출력.
bool CPlayer::Draw()
{
	if (true != flag) 
		return false;

	szScreenBuffer[Y][X] = 'A';
	return true;
}


//플레이어 체력이 0 이하인지 확인
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

//키보드 입력
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