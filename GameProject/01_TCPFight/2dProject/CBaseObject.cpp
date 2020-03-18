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

//외부에서의 액션 입력은 단순 멤버 변수에 셋팅만 해준다
//큐 방식은 사용할 필요x - 액션 게임이라
//여기서 직접 스프라이트 변경, 애니메이션 변경, 좌표이동을 해서는 안된다.
//실질적인 처리는 action 함수에서만 한다.
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

	//프레임 딜레이 값을 넘어야 다음 프레임으로 넘어간다
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
