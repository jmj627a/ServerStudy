#pragma once
#include "stdafx.h"
#include "CBaseObject.h"
class CPlayerObject;
extern CPlayerObject* g_pPlayerObject;
class CPlayerObject : public CBaseObject
{
private:
	bool m_bPlayerCharacter;
	char m_chHP;
	DWORD m_dwActionCur;
	DWORD m_dwActionOld;
	int m_iDirCur;	//�� �Ʒ��� �� ������ �˾ƾ���. ���ΰ��µ� ���ʺ����� �׷���
	int m_iDirOld;

public:
	CPlayerObject(bool _isPlayerCharacter, int _dir);
	~CPlayerObject();

	void ActionProc();
	int GetDirection();
	int GetCurAction();
	char GetHP();
	void InputActionProc();
	bool IsPlayer();

	void Render(BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch);
	DWORD Run();

	void SetActionAttack1();
	void SetActionAttack2();
	void SetActionAttack3();

	void SetActionMove(int action);
	void SetActionStand();
	
	void SetDirection(int _dir);
	void SetHP(char _hp);
};

