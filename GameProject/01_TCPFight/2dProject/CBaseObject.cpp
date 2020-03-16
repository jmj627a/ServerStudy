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
