#include "CPlayerObject.h"
#include "framework.h"

void CPlayerObject::Render(BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch)
{
	////�׸��� ��� 
	//g_cSprite.DrawSprite50(eSHADOW, _iCurX, _iCurY, bypDest, iDestWidth, iDestHeight, iDestPitch);
	//
	////�÷��̾� ��ü ���
	//g_cSprite.DrawSprite(GetSprite(), _iCurX, _iCurY, bypDest, iDestWidth, iDestHeight, iDestPitch);
	//
	////hp ������ �� ��� 
	//g_cSprite.DrawSprite(eGUAGE_HP, _iCurX - 35, _iCurY + 9, bypDest, iDestWidth, iDestHeight, iDestPitch, GetHP());
}

//action
DWORD CPlayerObject::Run()
{
	//���� ���������� �ִϸ��̼�
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
