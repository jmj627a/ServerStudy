#include "CPlayerObject.h"
#include "framework.h"

void CPlayerObject::Render(BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch)
{
	////그림자 출력 
	//g_cSprite.DrawSprite50(eSHADOW, _iCurX, _iCurY, bypDest, iDestWidth, iDestHeight, iDestPitch);
	//
	////플레이어 객체 출력
	//g_cSprite.DrawSprite(GetSprite(), _iCurX, _iCurY, bypDest, iDestWidth, iDestHeight, iDestPitch);
	//
	////hp 게이지 바 출력 
	//g_cSprite.DrawSprite(eGUAGE_HP, _iCurX - 35, _iCurY + 9, bypDest, iDestWidth, iDestHeight, iDestPitch, GetHP());
}

//action
DWORD CPlayerObject::Run()
{
	//다음 프레임으로 애니메이션
	NextFrame();
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

void CPlayerObject::InputActionProc()
{
	switch (m_dwActionCur)
	{
	case dfACTION_MOVE_DD:
		m_iCurY -= 5;
		break;

	case dfACTION_MOVE_LD:
		m_iCurX -= 5;
		m_iCurY -= 5;
		break;

	case dfACTION_MOVE_LL:
		m_iCurX -= 5; 
		break;

	case dfACTION_MOVE_LU:
		m_iCurX -= 5;
		m_iCurY += 5;
		break;

	case dfACTION_MOVE_RD:
		m_iCurX += 5;
		m_iCurY -= 5;
		break;

	case dfACTION_MOVE_RR:
		m_iCurX += 5;
		break;

	case dfACTION_MOVE_RU:
		m_iCurX += 5;
		m_iCurY -= 5;
		break;

	case dfACTION_MOVE_UU:
		m_iCurY -= 5; 
		break;
	}
}

void CPlayerObject::SetActionMove()
{
	m_dwActionOld = m_dwActionCur;
	m_dwActionCur = dfACTION_MOVE_LL;
}

void CPlayerObject::SetActionStand()
{
	m_dwActionOld = m_dwActionCur;
	m_dwActionCur = dfACTION_STAND;
}
