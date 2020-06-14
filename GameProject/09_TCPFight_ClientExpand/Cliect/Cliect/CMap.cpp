#include "stdafx.h"
#include "CMap.h"


CMap::CMap()
{
}


CMap::~CMap()
{
}

void CMap::setDrawPos(WORD _x, WORD _y)
{
	// iX의 최대점은 최대 X 사이즈 - dfSCREEN_WIDTH / 2
	if (_x > dfRANGE_MOVE_RIGHT - dfSCREEN_WIDTH / 2)
		m_DrawX = dfRANGE_MOVE_RIGHT - dfSCREEN_WIDTH / 2;
	else if (_x < dfSCREEN_WIDTH / 2)
		m_DrawX = 0;
	else
		m_DrawX = max(abs(_x - dfSCREEN_WIDTH / 2), 0);


	// iY의 최대점은 최대 Y 사이즈 - dfSCREEN_HEIGHT / 2
	if (_y > dfRANGE_MOVE_BOTTOM - dfSCREEN_HEIGHT / 2)
		m_DrawY = dfRANGE_MOVE_BOTTOM - dfSCREEN_HEIGHT / 2;
	else if (_y < dfSCREEN_HEIGHT / 2)
		m_DrawY = 0;
	else
		m_DrawY = max(abs(_y - dfSCREEN_HEIGHT / 2), 0);
}
