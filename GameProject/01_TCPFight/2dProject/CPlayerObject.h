#pragma once
#include <Windows.h>
#include "CBaseObject.h"
class CPlayerObject : public CBaseObject
{
private:
	bool m_bPlayerCharacter;
	int m_chHP;
	DWORD m_dwActionCur;
	DWORD m_dwActionOld;
	int m_iDirCur;
	int m_iDirOld;

public:
	CPlayerObject();
	~CPlayerObject();

	void ActionProc();
	int GetDirection();
	int GetHP();
	void InputActionProc();
	bool IsPlayer();

	void Render();
	void Run();

	void SetActionAttack1();
	void SetActionAttack2();
	void SetActionAttack3();

	void SetActionMove();
	void SetActionStand();
	
	void SetDirection();
	void SetHP();
};

