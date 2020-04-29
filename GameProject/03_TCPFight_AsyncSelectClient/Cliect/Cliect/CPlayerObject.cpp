#pragma once
#include "stdafx.h"
#include "CPlayerObject.h"
#include "CSpriteDib.h"
#include "CNetwork.h"

extern CSpriteDib g_cSprite;
extern CPlayerObject* g_pPlayerObject;
extern CNetwork *network;

void CPlayerObject::Render(BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch)
{
	//그림자 출력 
	g_cSprite.DrawSprite50(eSHADOW, m_iCurX, m_iCurY + 3, bypDest, iDestWidth, iDestHeight, iDestPitch);
	
	//플레이어 객체 출력
	if(m_iObjectID == g_pPlayerObject->GetObjectID())
		g_cSprite.DrawMySprite(GetSprite(), m_iCurX, m_iCurY, bypDest, iDestWidth, iDestHeight, iDestPitch);
	else
		g_cSprite.DrawSprite(GetSprite(), m_iCurX, m_iCurY, bypDest, iDestWidth, iDestHeight, iDestPitch);
	
	//hp 게이지 바 출력 
	g_cSprite.DrawSprite(eGUAGE_HP, m_iCurX - 30, m_iCurY + 10, bypDest, iDestWidth, iDestHeight, iDestPitch, GetHP());
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

	if (m_iDirCur == dfDIR_LEFT)
		SetSprite(ePLAYER_ATTACK1_L01, ePLAYER_ATTACK1_L_MAX, dfDELAY_ATTACK1);
	else
		SetSprite(ePLAYER_ATTACK1_R01, ePLAYER_ATTACK1_R_MAX, dfDELAY_ATTACK1);


	//sendPacket
}

void CPlayerObject::SetActionAttack2()
{
	m_dwActionOld = m_dwActionCur; 
	m_dwActionCur = dfACTION_ATTACK2;

	if (m_iDirCur == dfDIR_LEFT)
		SetSprite(ePLAYER_ATTACK2_L01, ePLAYER_ATTACK2_L_MAX, dfDELAY_ATTACK2);
	else
		SetSprite(ePLAYER_ATTACK2_R01, ePLAYER_ATTACK2_R_MAX, dfDELAY_ATTACK2);

	//sendPacket

}

void CPlayerObject::SetActionAttack3()
{
	m_dwActionOld = m_dwActionCur; 
	m_dwActionCur = dfACTION_ATTACK3;

	if (m_iDirCur == dfDIR_LEFT)
		SetSprite(ePLAYER_ATTACK3_L01, ePLAYER_ATTACK3_L_MAX, dfDELAY_ATTACK3);
	else
		SetSprite(ePLAYER_ATTACK3_R01, ePLAYER_ATTACK3_R_MAX, dfDELAY_ATTACK3);

	//sendPacket

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

int CPlayerObject::GetCurAction()
{
	return m_dwActionCur;
}

char CPlayerObject::GetHP()
{
	return m_chHP;
}

void CPlayerObject::InputActionProc()
{
	//오브젝트의 액션을 보내는것 
	//m_dwActionOld = m_dwActionCur;

	//오른쪽으로 돌았으면 그 메세지만 보냄. 행동의 변화가있을때만

	switch (m_dwActionInput)
	{
	case dfACTION_MOVE_DD:
		SetActionMove(dfACTION_MOVE_DD);
		m_iCurY += 2;
		break;

	case dfACTION_MOVE_LD:
		SetDirection(dfDIR_LEFT);
		SetActionMove(dfACTION_MOVE_LD);
		m_iCurX -= 3;
		m_iCurY += 2; 
		break;

	case dfACTION_MOVE_LL:
		SetDirection(dfDIR_LEFT);
		SetActionMove(dfACTION_MOVE_LL);
		m_iCurX -= 3;
		break;

	case dfACTION_MOVE_LU:
		SetDirection(dfDIR_LEFT);
		SetActionMove(dfACTION_MOVE_LU);
		m_iCurX -= 3;
		m_iCurY -= 2; 
		break;

	case dfACTION_MOVE_RD:
		SetDirection(dfDIR_RIGHT);
		SetActionMove(dfACTION_MOVE_RD);
		m_iCurX += 3;
		m_iCurY += 2; 
		break;

	case dfACTION_MOVE_RR:
		SetDirection(dfDIR_RIGHT);
		SetActionMove(dfACTION_MOVE_RR);
		m_iCurX += 3; 
		break;

	case dfACTION_MOVE_RU:
		SetDirection(dfDIR_RIGHT);
		SetActionMove(dfACTION_MOVE_RU);
		m_iCurX += 3;
		m_iCurY -= 2; 
		break;

	case dfACTION_MOVE_UU:
		SetActionMove(dfACTION_MOVE_UU);
		m_iCurY -= 2; 
		break;

	case dfACTION_STAND:
		SetActionStand();
		break;

	case dfACTION_ATTACK1:
		SetActionAttack1();
		break;

	case dfACTION_ATTACK2:
		SetActionAttack2();
		break;

	case dfACTION_ATTACK3:
		SetActionAttack3();
		break;
	}
}

bool CPlayerObject::IsPlayer()
{
	if (GetObjectID() == g_pPlayerObject->GetObjectID())
		return true;
	else
		return false;
}

void CPlayerObject::SetActionMove(int action)
{
	m_dwActionOld = m_dwActionCur;
	m_dwActionCur = action;

	switch (m_dwActionCur)
	{
	case dfACTION_MOVE_DD:
		//이전 행동이 멈춘거면 이동
		if (m_dwActionOld == dfACTION_STAND)
		{
			if(m_iDirCur == dfDIR_LEFT)
				SetSprite(ePLAYER_MOVE_L01, ePLAYER_MOVE_L_MAX, dfDELAY_MOVE);
			else
				SetSprite(ePLAYER_MOVE_R01, ePLAYER_MOVE_R_MAX, dfDELAY_MOVE);
		}
		break;

	case dfACTION_MOVE_LD:
		//방향이 다르면 무조건 스프라이트 처음부터
		if (m_iDirOld != dfDIR_LEFT)
			SetSprite(ePLAYER_MOVE_L01, ePLAYER_MOVE_L_MAX, dfDELAY_MOVE);
		//방향이 같다면 걷기중이 아니었다면 처음부터. 그 전동작이 stand였다면 처음부터 
		if(m_dwActionOld == dfACTION_STAND)
			SetSprite(ePLAYER_MOVE_L01, ePLAYER_MOVE_L_MAX, dfDELAY_MOVE);
		break;

	case dfACTION_MOVE_LL:
		if (m_iDirOld != dfDIR_LEFT)
			SetSprite(ePLAYER_MOVE_L01, ePLAYER_MOVE_L_MAX, dfDELAY_MOVE);
		if (m_dwActionOld == dfACTION_STAND)
			SetSprite(ePLAYER_MOVE_L01, ePLAYER_MOVE_L_MAX, dfDELAY_MOVE);
		break;

	case dfACTION_MOVE_LU:
		if (m_iDirOld != dfDIR_LEFT)
			SetSprite(ePLAYER_MOVE_L01, ePLAYER_MOVE_L_MAX, dfDELAY_MOVE);
		if (m_dwActionOld == dfACTION_STAND)
			SetSprite(ePLAYER_MOVE_L01, ePLAYER_MOVE_L_MAX, dfDELAY_MOVE); 
		break;

	case dfACTION_MOVE_RD:
		if(m_iDirOld != dfDIR_RIGHT)
			SetSprite(ePLAYER_MOVE_R01, ePLAYER_MOVE_R_MAX, dfDELAY_MOVE);
		if (m_dwActionOld == dfACTION_STAND)
			SetSprite(ePLAYER_MOVE_R01, ePLAYER_MOVE_R_MAX, dfDELAY_MOVE);
		break;

	case dfACTION_MOVE_RR:
		if (m_iDirOld != dfDIR_RIGHT)
			SetSprite(ePLAYER_MOVE_R01, ePLAYER_MOVE_R_MAX, dfDELAY_MOVE);
		if (m_dwActionOld == dfACTION_STAND)
			SetSprite(ePLAYER_MOVE_R01, ePLAYER_MOVE_R_MAX, dfDELAY_MOVE); 
		break;

	case dfACTION_MOVE_RU:
		if (m_iDirOld != dfDIR_RIGHT)
			SetSprite(ePLAYER_MOVE_R01, ePLAYER_MOVE_R_MAX, dfDELAY_MOVE);
		if (m_dwActionOld == dfACTION_STAND)
			SetSprite(ePLAYER_MOVE_R01, ePLAYER_MOVE_R_MAX, dfDELAY_MOVE); 
		break;

	case dfACTION_MOVE_UU:
		if (m_dwActionOld == dfACTION_STAND)
		{
			if (m_iDirCur == dfDIR_LEFT)
				SetSprite(ePLAYER_MOVE_L01, ePLAYER_MOVE_L_MAX, dfDELAY_MOVE);
			else
				SetSprite(ePLAYER_MOVE_R01, ePLAYER_MOVE_R_MAX, dfDELAY_MOVE);
		}
		break;
	}

	//이전과 행동이 바뀌었으면 패킷 보내기
	if (IsPlayer() && m_dwActionOld != m_dwActionCur && m_dwActionCur != dfACTION_STAND)
	{
		stNETWORK_PACKET_HEADER Header;
		stPACKET_CS_MOVE_START Packet;

		network->mpMoveStart(&Header, &Packet, GetCurAction(), GetCurX(), GetCurY());
		network->SendPacket(&Header, (char*)&Packet);
	}

}

void CPlayerObject::SetActionStand()
{
	m_dwActionOld = m_dwActionCur;
	m_dwActionCur = dfACTION_STAND;

	if (m_dwActionOld == dfACTION_STAND)
		return;

	if (m_iDirCur == dfDIR_LEFT)
		SetSprite(ePLAYER_STAND_L01, ePLAYER_STAND_L_MAX, dfDELAY_STAND);
	else if (m_iDirCur == dfDIR_RIGHT)
		SetSprite(ePLAYER_STAND_R01, ePLAYER_STAND_R_MAX, dfDELAY_STAND);


	if (IsPlayer() && m_dwActionOld != m_dwActionCur )
	{
		stNETWORK_PACKET_HEADER Header;
		stPACKET_CS_MOVE_STOP Packet;

		network->mpMoveStop(&Header, &Packet, GetDirection(), GetCurX(), GetCurY());
		network->SendPacket(&Header, (char*)&Packet);
	}
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

void CPlayerObject::SetcurAction(int _action)
{
	m_dwActionOld = m_dwActionCur;
	m_dwActionCur = _action;
}

void CPlayerObject::SetHP(char _hp)
{
	m_chHP = _hp;
}
