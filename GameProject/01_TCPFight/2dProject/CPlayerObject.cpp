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

	switch (m_dwActionCur)
	{
	case dfACTION_MOVE_DD:
		break;

	case dfACTION_MOVE_LD:
		break;

	case dfACTION_MOVE_LL:
		break;

	case dfACTION_MOVE_LU:
		break;

	case dfACTION_MOVE_RD:
		break;

	case dfACTION_MOVE_RR:
		break;

	case dfACTION_MOVE_RU:
		break;

	case dfACTION_MOVE_UU:
		break;

	case dfACTION_STAND:
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

void CPlayerObject::SetDirection(int _dir)
{
	m_iDirOld = m_iDirCur;
	
	//왼쪽
	if (_dir == 0)
		m_iDirCur = 0;
	//오른쪽
	else if (_dir == 1)
		m_iDirCur = 1;
}

void CPlayerObject::SetHP(char _hp)
{
	m_chHP = _hp;
}
