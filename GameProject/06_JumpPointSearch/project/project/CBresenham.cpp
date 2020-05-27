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

		//���ΰ� �� �� ���, ���θ� 1�� ������Ű�鼭 error += addNum;
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
		//���ΰ� �� �� ���, ���θ� 1�� ������Ű�鼭 error += addNum;
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

	//���� ��
	if (xLength >= yLength)
	{
		addNum = yLength; //���� ���� ��� �����ؼ� ���� �� �� ��
		errorMax = xLength;
		return false;
	}
	//���� ��
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

	for (iter; iter != dotList.end(); ++iter)
	{
		g_Grid[(*iter).m_iy][(*iter).m_ix].grid_type = eLINE;
		//HBRUSH Brush, oBrush;
		//Brush = CreateSolidBrush(RGB(255, 0, 0));
		//oBrush = (HBRUSH)SelectObject(hdc, Brush);
		//Rectangle(hdc, (*iter).m_ix, (*iter).m_iy, (*iter).m_ix + 20, (*iter).m_iy + 20);
		//SelectObject(hdc, oBrush);
		//DeleteObject(Brush);
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
