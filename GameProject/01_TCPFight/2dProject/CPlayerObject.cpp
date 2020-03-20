#include "CPlayerObject.h"
#include "framework.h"
#include "CSpriteDib.h"

extern CSpriteDib g_cSprite;

void CPlayerObject::Render(BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch)
{
	//�׸��� ��� 
	g_cSprite.DrawSprite50(eSHADOW, m_iCurX + 32, m_iCurY + 88, bypDest, iDestWidth, iDestHeight, iDestPitch);
	
	//�÷��̾� ��ü ���
	if(m_iObjectID == 0)
		g_cSprite.DrawMySprite(GetSprite(), m_iCurX, m_iCurY, bypDest, iDestWidth, iDestHeight, iDestPitch);
	else
		g_cSprite.DrawSprite(GetSprite(), m_iCurX, m_iCurY, bypDest, iDestWidth, iDestHeight, iDestPitch);
	
	//hp ������ �� ��� 
	g_cSprite.DrawSprite(eGUAGE_HP, m_iCurX + 30, m_iCurY + 100, bypDest, iDestWidth, iDestHeight, iDestPitch, GetHP());
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

char CPlayerObject::GetHP()
{
	return m_chHP;
}

void CPlayerObject::InputActionProc()
{
	//������Ʈ�� �׼��� �����°� 
	m_dwActionOld = m_dwActionCur;

	//���������� �������� �� �޼����� ����. �ൿ�� ��ȭ����������

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
		//������ �ٸ��� ������ ��������Ʈ ó������
		if (m_iDirOld != dfDIR_LEFT)
			SetSprite(ePLAYER_MOVE_L01, ePLAYER_MOVE_L_MAX, dfDELAY_STAND);
		//������ ���ٸ� �ȱ����� �ƴϾ��ٸ� ó������. �� �������� stand���ٸ� ó������ 
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
	
	//����
	if (_dir == dfDIR_LEFT)
		m_iDirCur = dfDIR_LEFT;
	//������
	else if (_dir == dfDIR_RIGHT)
		m_iDirCur = dfDIR_RIGHT;
}

void CPlayerObject::SetHP(char _hp)
{
	m_chHP = _hp;
}
