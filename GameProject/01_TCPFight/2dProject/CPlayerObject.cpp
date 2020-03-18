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
	
	//����
	if (_dir == 0)
		m_iDirCur = 0;
	//������
	else if (_dir == 1)
		m_iDirCur = 1;
}

void CPlayerObject::SetHP(char _hp)
{
	m_chHP = _hp;
}
