#include "pch.h"
#include "CBresenham.h"
#include <stdlib.h>

extern TILE g_Grid[WORLD_HEIGHT][WORLD_WIDTH];

CBresenham::CBresenham()
{
}


CBresenham::~CBresenham()
{
}

bool CBresenham::checkDot()
{
	bool longerSide = compareLength();

	int x = startPos.m_ix;
	int y = startPos.m_iy;

	while (true)
	{
		if (x == endPos.m_ix && y == endPos.m_iy)
		{
			dotList.push_back(endPos);
			return true;
		}

		//가로가 더 긴 경우, 가로를 1씩 증가시키면서 error += addNum;
		if (longerSide == 0)
		{
			x += deltaX;
			error += addNum;

			if (error >= errorMax)
			{
				y += deltaY;
				error -= errorMax;
			}
		}
		//세로가 더 긴 경우, 세로를 1씩 증가시키면서 error += addNum;
		else
		{
			y += deltaY;
			error += addNum;

			if (error >= errorMax)
			{
				x += deltaX;
				error -= errorMax;
			}
		}

		if (g_Grid[y][x].grid_type == eBLOCKED)
			return false;

		dotList.push_back(POS(x, y));
	}

}

bool CBresenham::compareLength()
{
	int xLength = startPos.m_ix - endPos.m_ix;
	int yLength = startPos.m_iy - endPos.m_iy;

	deltaX = (xLength < 0) ? 1 : ((xLength == 0) ? 0 : -1);
	deltaY = (yLength < 0) ? 1 : ((yLength == 0) ? 0 : -1);

	xLength = abs(xLength);
	yLength = abs(yLength);

	//가로 김
	if (xLength >= yLength)
	{
		addNum = yLength; //상대방 값을 계속 누적해서 더할 때 쓸 것
		errorMax = xLength;
		return false;
	}
	//세로 김
	else
	{
		addNum = xLength;
		errorMax = yLength;
		return true;
	}
}

void CBresenham::LineDraw(HWND hWnd)
{
	if (dotList.size() == 0)
		return;

	HDC hdc = GetDC(hWnd);
	
	std::list<POS>::iterator iter = dotList.begin();
	std::list<POS>::iterator nextiter = dotList.begin();
	++nextiter;

	for (nextiter; nextiter != dotList.end(); ++nextiter)
	{
		HPEN Pen, oPen;
		Pen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
		oPen = (HPEN)SelectObject(hdc, Pen);
		MoveToEx(hdc, (*iter).m_ix * 20 + 10, (*iter).m_iy * 20 + 10, NULL);
		LineTo(hdc, (*nextiter).m_ix * 20 + 10, (*nextiter).m_iy * 20 + 10);
		SelectObject(hdc, oPen);
		DeleteObject(Pen);

		++iter;
	}

	InvalidateRect(hWnd, NULL, false);
	ReleaseDC(hWnd, hdc);
}

void CBresenham::setPos(int _startPosX, int _startPosY, int _endPosX, int _endPosY)
{
	startPos.m_ix = _startPosX;
	startPos.m_iy = _startPosY;
	
	endPos.m_ix = _endPosX;
	endPos.m_iy = _endPosY;
}
