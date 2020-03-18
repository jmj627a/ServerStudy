#include "CBaseObject.h"

void CBaseObject::SetSprite(int iSpriteStart, int iSpriteMax, int iFrameDelay)
{
	m_iSpriteStart = iSpriteStart;
	m_iSpriteMax = iSpriteMax;
	m_iFrameDelay = iFrameDelay;

	m_iSpriteNow = iSpriteStart;
	m_iDelayCount = 0;
	m_bEndFrame = FALSE;
}

void CBaseObject::SetPosition(int _CurX, int _CurY)
{
	m_iCurX = _CurX;
	m_iCurY = _CurY;
}

CBaseObject::CBaseObject()
{
}

CBaseObject::~CBaseObject()
{
}

//�ܺο����� �׼� �Է��� �ܼ� ��� ������ ���ø� ���ش�
//ť ����� ����� �ʿ�x - �׼� �����̶�
//���⼭ ���� ��������Ʈ ����, �ִϸ��̼� ����, ��ǥ�̵��� �ؼ��� �ȵȴ�.
//�������� ó���� action �Լ������� �Ѵ�.
void CBaseObject::ActionInput(DWORD dwAction)
{
	m_dwActionInput = dwAction;
}

int CBaseObject::GetCurX(void)
{
	return m_iCurX;
}

int CBaseObject::GetCurY(void)
{
	return m_iCurY;
}

int CBaseObject::GetObjectID(void)
{
	return m_iObjectID;
}

int CBaseObject::GetSprite(void)
{
	return m_iSpriteNow;
}

int CBaseObject::IsEndFrame(void)
{
	return m_bEndFrame;
}

void CBaseObject::NextFrame(void)
{
	if (0 > m_iSpriteStart)
		return;

	//������ ������ ���� �Ѿ�� ���� ���������� �Ѿ��
	m_iDelayCount++;

	if (m_iDelayCount >= m_iFrameDelay)
	{
		m_iDelayCount = 0;
		m_iSpriteNow++;

		if (m_iSpriteNow > m_iSpriteMax)
		{
			m_iSpriteNow = m_iSpriteStart;
			m_bEndFrame = TRUE;
		}
	}
}

void CBaseObject::Render(BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch)
{
}

DWORD CBaseObject::Run()
{
	return 0;
}
