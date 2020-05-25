#include "pch.h"
#include "CBresenham.h"
#include <stdlib.h>

CBresenham::CBresenham()
{
}


CBresenham::~CBresenham()
{
}

void CBresenham::checkDot()
{
	bool longerSide = compareLength();

	int x = startPos.m_ix;
	int y = startPos.m_iy;

	while (true)
	{
		if (x == endPos.m_ix && y == endPos.m_iy)
		{
			dotList.push_back(endPos);
			return;
		}

		//���ΰ� �� �� ���, ���θ� 1�� ������Ű�鼭 error += addNum;
		if (longerSide == 0)
		{
			x += 1;
			error += addNum;

			if (error >= 10)
			{
				y += 1;
				error -= 10;
			}
		}
		//���ΰ� �� �� ���, ���θ� 1�� ������Ű�鼭 error += addNum;
		else
		{
			y += 1;
			error += addNum;

			if (error >= 10)
			{
				x += 1;
				error -= 10;
			}
		}

		dotList.push_back(POS(x, y));
	}

}

bool CBresenham::compareLength()
{
	int xLength;
	int yLength;

	xLength = abs(startPos.m_ix - endPos.m_ix);
	yLength = abs(startPos.m_iy - endPos.m_iy);

	//���� ��
	if (xLength >= yLength)
	{
		addNum = yLength; //���� ���� ��� �����ؼ� ���� �� �� ��
		return false;
	}
	//���� ��
	else
	{
		addNum = xLength;
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