#pragma once

#define dfSCREEN_WIDTH 640
#define dfSCREEN_HEIGHT 480


class CMap
{

public:
	WORD m_DrawX;
	WORD m_DrawY;

	CMap();
	~CMap();

	void setDrawPos(WORD _x, WORD _y);

};

