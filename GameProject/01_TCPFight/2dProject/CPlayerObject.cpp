#include "CPlayerObject.h"
#include "framework.h"
#include "CSpriteDib.h"

extern CSpriteDib g_cSprite;

void CPlayerObject::Render(BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch)
{
	//그림자 출력 
	g_cSprite.DrawSprite50(eSHADOW, m_iCurX + 32, m_iCurY + 88, bypDest, iDestWidth, iDestHeight, iDestPitch);
	
	//플레이어 객체 출력
	if(m_iObjectID == 0)
		g_cSprite.DrawMySprite(GetSprite(), m_iCurX, m_iCurY, bypDest, iDestWidth, iDestHeight, iDestPitch);
	else
		g_cSprite.DrawSprite(GetSprite(), m_iCurX, m_iCurY, bypDest, iDestWidth, iDestHeight, iDestPitch);
	
	//hp 게이지 바 출력 
	g_cSprite.DrawSprite(eGUAGE_HP, m_iCurX + 30, m_iCurY + 100, bypDest, iDestWidth, iDestHeight, iDestPitch, GetHP());
}

//action
DWORD CPlayerObject::Run()
{
	//다음 프레임으로 애니메이션
	NextFrame(); //빨리넘어가면 빨리넘어가서 로직의 일부로 들어간것
	ActionProc();

	return FALSE;
}

void CPlayerObject::SetActionAttack1()
{
	m_dwActionOld = m_dwActionCur; 
	m_dwActionCur = dfACTION_ATTACK1;
}

void CPlayerObject::SetActionAttack2()
{
	m_dwActionOld = m_dwActionCur; 
	m_dwActionCur = dfACTION_ATTACK2;
}

void CPlayerObject::SetActionAttack3()
{
	m_dwActionOld = m_dwActionCur; 
	m_dwActionCur = dfACTION_ATTACK3;
}

CPlayerObject::CPlayerObject()
{
	m_bPlayerCharacter = true;
	m_chHP = 100;
	m_dwActionCur = dfACTION_STAND;
	m_dwActionOld = dfACTION_STAND;
	m_iDirCur = 0;	//위 아래는 얼굴 방향을 알아야함. 위로가는데 왼쪽보는지 그런거
	m_iDirOld = 0;
}

CPlayerObject::~CPlayerObject()
{
}

void CPlayerObject::ActionProc()
{
	//몇몇 동작 시(공격)의 경우 강제적으로 해당 동작 처리를 완료해야만 한다

	switch (m_dwActionCur)
	{
		//공격동작 또는 데미지 동작은 애니메이션이 끝날 때까지 강제적으로 애니메이션이 되어야만 하며
		//애니메이션이 끝난 후 기본동작으로 자동으로 돌아가야 한다.

	case dfACTION_ATTACK1:
	case dfACTION_ATTACK2:
	case dfACTION_ATTACK3:
		if (IsEndFrame())
		{
			//스프라이트 바꿔주기
			SetActionStand();
			//공격이 끝났으면 액션을 바꿔 연속으로 공격을 할 때 재 전송이 가능하도록
			m_dwActionInput = dfACTION_STAND;
		}
		break;

	default:
		//이외의 경우에는 사용자 입력 처리를 해준다
		InputActionProc();
		break;
	}
}

int CPlayerObject::GetDirection()
{
	return m_iDirCur;
}

char CPlayerObject::GetHP()
{
	return m_chHP;
}

void CPlayerObject::InputActionProc()
{
	//오브젝트의 액션을 보내는것 
	m_dwActionOld = m_dwActionCur;

	//오른쪽으로 돌았으면 그 메세지만 보냄. 행동의 변화가있을때만

	switch (m_dwActionInput)
	{
	case dfACTION_MOVE_DD:
		SetActionMove(dfACTION_MOVE_DD);
		break;

	case dfACTION_MOVE_LD:
		SetDirection(dfDIR_LEFT);
		SetActionMove(dfACTION_MOVE_LD);
		break;

	case dfACTION_MOVE_LL:
		SetDirection(dfDIR_LEFT);
		SetActionMove(dfACTION_MOVE_LL);
		break;

	case dfACTION_MOVE_LU:
		SetDirection(dfDIR_LEFT);
		SetActionMove(dfACTION_MOVE_LU);
		break;

	case dfACTION_MOVE_RD:
		SetDirection(dfDIR_RIGHT);
		SetActionMove(dfACTION_MOVE_RD);
		break;

	case dfACTION_MOVE_RR:
		SetDirection(dfDIR_RIGHT);
		SetActionMove(dfACTION_MOVE_RR);
		break;

	case dfACTION_MOVE_RU:
		SetDirection(dfDIR_RIGHT);
		SetActionMove(dfACTION_MOVE_RU);
		break;

	case dfACTION_MOVE_UU:
		SetActionMove(dfACTION_MOVE_UU);
		break;

	case dfACTION_STAND:
		SetActionStand();
		break;

	case dfACTION_ATTACK1:
		SetActionAttack1();
		SetSprite(ePLAYER_STAND_R01, ePLAYER_STAND_R_MAX, dfDELAY_STAND);
		break;

	case dfACTION_ATTACK2:
		SetActionAttack2();
		SetSprite(ePLAYER_STAND_R01, ePLAYER_STAND_R_MAX, dfDELAY_STAND);
		break;

	case dfACTION_ATTACK3:
		SetActionAttack3();
		SetSprite(ePLAYER_STAND_R01, ePLAYER_STAND_R_MAX, dfDELAY_STAND);
		break;
	}
}

void CPlayerObject::SetActionMove(int action)
{
	m_dwActionOld = m_dwActionCur;
	m_dwActionCur = action;

	switch (m_dwActionCur)
	{
	case dfACTION_MOVE_DD:
		if (m_dwActionOld != dfACTION_MOVE_DD)
		{
		}
		m_iCurY += 2;
		break;

	case dfACTION_MOVE_LD:
		//방향이 다르면 무조건 스프라이트 처음부터
		if (m_iDirOld != dfDIR_LEFT)
			SetSprite(ePLAYER_MOVE_L01, ePLAYER_MOVE_L_MAX, dfDELAY_STAND);
		//방향이 같다면 걷기중이 아니었다면 처음부터. 그 전동작이 stand였다면 처음부터 
		if(m_dwActionOld == dfACTION_STAND)
			SetSprite(ePLAYER_MOVE_L01, ePLAYER_MOVE_L_MAX, dfDELAY_STAND);

		m_iCurX -= 3;
		m_iCurY += 2;
		break;

	case dfACTION_MOVE_LL:
		if (m_iDirOld != dfDIR_LEFT)
			SetSprite(ePLAYER_MOVE_L01, ePLAYER_MOVE_L_MAX, dfDELAY_STAND);
		if (m_dwActionOld == dfACTION_STAND)
			SetSprite(ePLAYER_MOVE_L01, ePLAYER_MOVE_L_MAX, dfDELAY_STAND); 
		
		m_iCurX -= 3;
		break;

	case dfACTION_MOVE_LU:
		if (m_iDirOld != dfDIR_LEFT)
			SetSprite(ePLAYER_MOVE_L01, ePLAYER_MOVE_L_MAX, dfDELAY_STAND);
		if (m_dwActionOld == dfACTION_STAND)
			SetSprite(ePLAYER_MOVE_L01, ePLAYER_MOVE_L_MAX, dfDELAY_STAND); 
		
		m_iCurX -= 3;
		m_iCurY -= 2; 
		break;

	case dfACTION_MOVE_RD:
		if(m_iDirOld != dfDIR_RIGHT)
			SetSprite(ePLAYER_MOVE_R01, ePLAYER_MOVE_R_MAX, dfDELAY_STAND);
		if (m_dwActionOld == dfACTION_STAND)
			SetSprite(ePLAYER_MOVE_R01, ePLAYER_MOVE_R_MAX, dfDELAY_STAND);
		
		m_iCurX += 3;
		m_iCurY += 2; 
		break;

	case dfACTION_MOVE_RR:
		if (m_iDirOld != dfDIR_RIGHT)
			SetSprite(ePLAYER_MOVE_R01, ePLAYER_MOVE_R_MAX, dfDELAY_STAND);
		if (m_dwActionOld == dfACTION_STAND)
			SetSprite(ePLAYER_MOVE_R01, ePLAYER_MOVE_R_MAX, dfDELAY_STAND); 
		
		m_iCurX += 3;
		break;

	case dfACTION_MOVE_RU:
		if (m_iDirOld != dfDIR_RIGHT)
			SetSprite(ePLAYER_MOVE_R01, ePLAYER_MOVE_R_MAX, dfDELAY_STAND);
		if (m_dwActionOld == dfACTION_STAND)
			SetSprite(ePLAYER_MOVE_R01, ePLAYER_MOVE_R_MAX, dfDELAY_STAND); 
		
		m_iCurX += 3;
		m_iCurY -= 2; 
		break;

	case dfACTION_MOVE_UU:
		SetSprite(ePLAYER_STAND_R01, ePLAYER_STAND_R_MAX, dfDELAY_STAND);
		m_iCurY -= 2;
		break;
	}
}

void CPlayerObject::SetActionStand()
{
	m_dwActionOld = m_dwActionCur;
	m_dwActionCur = dfACTION_STAND;

	if (m_iDirCur == dfDIR_LEFT)
		SetSprite(ePLAYER_STAND_L01, ePLAYER_STAND_R_MAX, dfDELAY_STAND);
	else if (m_iDirCur == dfDIR_RIGHT)
		SetSprite(ePLAYER_STAND_R01, ePLAYER_STAND_R_MAX, dfDELAY_STAND);
}

void CPlayerObject::SetDirection(int _dir)
{
	m_iDirOld = m_iDirCur;
	
	//왼쪽
	if (_dir == dfDIR_LEFT)
		m_iDirCur = dfDIR_LEFT;
	//오른쪽
	else if (_dir == dfDIR_RIGHT)
		m_iDirCur = dfDIR_RIGHT;
}

void CPlayerObject::SetHP(char _hp)
{
	m_chHP = _hp;
}
