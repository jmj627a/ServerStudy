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
	//�׸��� ��� 
	g_cSprite.DrawSprite50(eSHADOW, m_iCurX, m_iCurY + 3, bypDest, iDestWidth, iDestHeight, iDestPitch);
	
	//�÷��̾� ��ü ���
	if(m_iObjectID == g_pPlayerObject->GetObjectID())
		g_cSprite.DrawMySprite(GetSprite(), m_iCurX, m_iCurY, bypDest, iDestWidth, iDestHeight, iDestPitch);
	else
		g_cSprite.DrawSprite(GetSprite(), m_iCurX, m_iCurY, bypDest, iDestWidth, iDestHeight, iDestPitch);
	
	//hp ������ �� ��� 
	g_cSprite.DrawSprite(eGUAGE_HP, m_iCurX - 30, m_iCurY + 10, bypDest, iDestWidth, iDestHeight, iDestPitch, GetHP());
}

//action
DWORD CPlayerObject::Run()
{
	//���� ���������� �ִϸ��̼�
	NextFrame(); //�����Ѿ�� �����Ѿ�� ������ �Ϻη� ����
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
	m_iDirCur = 0;	//�� �Ʒ��� �� ������ �˾ƾ���. ���ΰ��µ� ���ʺ����� �׷���
	m_iDirOld = 0;
}

CPlayerObject::~CPlayerObject()
{
}

void CPlayerObject::ActionProc()
{
	//��� ���� ��(����)�� ��� ���������� �ش� ���� ó���� �Ϸ��ؾ߸� �Ѵ�

	switch (m_dwActionCur)
	{
		//���ݵ��� �Ǵ� ������ ������ �ִϸ��̼��� ���� ������ ���������� �ִϸ��̼��� �Ǿ�߸� �ϸ�
		//�ִϸ��̼��� ���� �� �⺻�������� �ڵ����� ���ư��� �Ѵ�.

	case dfACTION_ATTACK1:
	case dfACTION_ATTACK2:
	case dfACTION_ATTACK3:
		if (IsEndFrame())
		{
			//��������Ʈ �ٲ��ֱ�
			SetActionStand();
			//������ �������� �׼��� �ٲ� �������� ������ �� �� �� ������ �����ϵ���
			m_dwActionInput = dfACTION_STAND;
		}
		break;

	default:
		//�̿��� ��쿡�� ����� �Է� ó���� ���ش�
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
	//������Ʈ�� �׼��� �����°� 
	//m_dwActionOld = m_dwActionCur;

	//���������� �������� �� �޼����� ����. �ൿ�� ��ȭ����������

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
		//���� �ൿ�� ����Ÿ� �̵�
		if (m_dwActionOld == dfACTION_STAND)
		{
			if(m_iDirCur == dfDIR_LEFT)
				SetSprite(ePLAYER_MOVE_L01, ePLAYER_MOVE_L_MAX, dfDELAY_MOVE);
			else
				SetSprite(ePLAYER_MOVE_R01, ePLAYER_MOVE_R_MAX, dfDELAY_MOVE);
		}
		break;

	case dfACTION_MOVE_LD:
		//������ �ٸ��� ������ ��������Ʈ ó������
		if (m_iDirOld != dfDIR_LEFT)
			SetSprite(ePLAYER_MOVE_L01, ePLAYER_MOVE_L_MAX, dfDELAY_MOVE);
		//������ ���ٸ� �ȱ����� �ƴϾ��ٸ� ó������. �� �������� stand���ٸ� ó������ 
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

	//������ �ൿ�� �ٲ������ ��Ŷ ������
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
	
	//����
	if (_dir == dfDIR_LEFT)
		m_iDirCur = dfDIR_LEFT;
	//������
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
