#pragma once
#include "pch.h"
#include "CAStar.h"

extern char g_Grid[30][50];


bool FComp(const NODE* lhs, const NODE* rhs) {
	return lhs->F < rhs->F;
};


CAStar::CAStar()
{

}


CAStar::~CAStar()
{
}

void CAStar::setG(NODE* _node)
{
	if (_node->pParent != nullptr)
		_node->G = _node->pParent->G + 1;
	else
		_node->G = 0;
}

void CAStar::setG_dia(NODE * _node)
{
	if (_node->pParent != nullptr)
		_node->G = _node->pParent->G + 1.5;
	else
		_node->G = 0;
}

void CAStar::compareG(NODE* _node)
{

}

void CAStar::setH(NODE * _node)
{
	_node->H = abs(_node->ix - iendX) + abs(_node->iy - iendY);
}

void CAStar::setF(NODE * _node)
{
	_node->F = _node->G + _node->H;
}

void CAStar::setEndPos(int x, int y)
{
	iendX = x;
	iendY = y;
}


void CAStar::searchLoad(HWND hWnd)
{
	while (true)
	{
		if (openList.size() == 0)
			return;
	
		openList.sort(FComp);
		NODE* popNode = openList.front();
		openList.pop_front();
	
		//길찾기 끝
		if (popNode->ix == iendX && popNode->iy == iendY)
		{
			endNode = popNode;
			return;
		}

		closeList.push_back(popNode);
		int x = popNode->ix;
		int y = popNode->iy;
		g_Grid[y][x] = eCLOSE;


		//■□□
		//□□□
		//□□□
		if (x - 1 >= 0 && y - 1 >= 0) //배열 다음줄로 밀리는경우
		{
			if (g_Grid[y - 1][x - 1] != eBLOCKED)
			{
				if (g_Grid[y - 1][x - 1] == eOPEN || g_Grid[y - 1][x - 1] == eCLOSE)
				{
					//노드는 만들지 않는데, 여기 해당하는 노드의 F값을 비교해서 더 빠른길이면 새로 넣어주기
				}
				else if ((g_Grid[y - 1][x - 1] == eBLANK) || (g_Grid[y - 1][x - 1] == eEND))
				{
					NODE* newNode = new NODE(x - 1, y - 1, popNode);
					setG_dia(newNode);
					setH(newNode);
					setF(newNode);
					openList.push_back(newNode);
					g_Grid[y - 1][x - 1] = eOPEN;
				}
			}
		}
		//□■□
		//□□□
		//□□□
		if (y - 1 >= 0)
		{
			if (g_Grid[y - 1][x] != eBLOCKED)
			{
				if (g_Grid[y - 1][x] == eOPEN || g_Grid[y - 1][x] == eCLOSE)
				{
					//노드는 만들지 않는데, 여기 해당하는 노드의 F값을 비교해서 더 빠른길이면 새로 넣어주기
				}
				else if ((g_Grid[y - 1][x] == eBLANK) || (g_Grid[y - 1][x] == eEND))
				{
					NODE* newNode = new NODE(x, y - 1, popNode);
					setG(newNode);
					setH(newNode);
					setF(newNode);
					openList.push_back(newNode);
					g_Grid[y - 1][x] = eOPEN;
				}
			}
		}
		//□□■
		//□□□
		//□□□
		if (x + 1 <= 50 && y - 1 >= 0)
		{
			if (g_Grid[y - 1][x + 1] != eBLOCKED)
			{
				if (g_Grid[y - 1][x + 1] == eOPEN || g_Grid[y - 1][x + 1] == eCLOSE)
				{
					//노드는 만들지 않는데, 여기 해당하는 노드의 F값을 비교해서 더 빠른길이면 새로 넣어주기
				}
				else if ((g_Grid[y - 1][x + 1] == eBLANK) || (g_Grid[y - 1][x + 1] == eEND))
				{
					NODE* newNode = new NODE(x + 1, y - 1, popNode);
					setG_dia(newNode);
					setH(newNode);
					setF(newNode);
					openList.push_back(newNode);
					g_Grid[y - 1][x + 1] = eOPEN;
				}
			}
		}
		//□□□
		//■□□
		//□□□
		if (x - 1 >= 0)
		{
			if (g_Grid[y][x - 1] != eBLOCKED)
			{
				if (g_Grid[y][x - 1] == eOPEN || g_Grid[y][x - 1] == eCLOSE)
				{
					//노드는 만들지 않는데, 여기 해당하는 노드의 F값을 비교해서 더 빠른길이면 새로 넣어주기
				}
				else if ((g_Grid[y][x - 1] == eBLANK) || (g_Grid[y][x - 1] == eEND))
				{
					NODE* newNode = new NODE(x - 1, y, popNode);
					setG(newNode);
					setH(newNode);
					setF(newNode);
					openList.push_back(newNode);
					g_Grid[y][x - 1] = eOPEN;
				}
			}
		}
		//□□□
		//□□■
		//□□□
		if (x + 1 <= 50)
		{
			if (g_Grid[y][x + 1] != eBLOCKED)
			{
				if (g_Grid[y][x + 1] == eOPEN || g_Grid[y][x + 1] == eCLOSE)
				{
					//노드는 만들지 않는데, 여기 해당하는 노드의 F값을 비교해서 더 빠른길이면 새로 넣어주기
				}
				else if ((g_Grid[y][x + 1] == eBLANK) || (g_Grid[y][x + 1] == eEND))
				{
					NODE* newNode = new NODE(x + 1, y, popNode);
					setG(newNode);
					setH(newNode);
					setF(newNode);
					openList.push_back(newNode);
					g_Grid[y][x + 1] = eOPEN;
				}
			}
		}
		//□□□
		//□□□
		//■□□
		if (x - 1 >= 0 && y + 1 <= 30)
		{
			if (g_Grid[y + 1][x - 1] != eBLOCKED)
			{
				if (g_Grid[y + 1][x - 1] == eOPEN || g_Grid[y + 1][x - 1] == eCLOSE)
				{
					//노드는 만들지 않는데, 여기 해당하는 노드의 F값을 비교해서 더 빠른길이면 새로 넣어주기
				}
				else if ((g_Grid[y + 1][x - 1] == eBLANK) || (g_Grid[y + 1][x - 1] == eEND))
				{
					NODE* newNode = new NODE(x - 1, y + 1, popNode);
					setG_dia(newNode);
					setH(newNode);
					setF(newNode);
					openList.push_back(newNode);
					g_Grid[y + 1][x - 1] = eOPEN;
				}
			}
		}
		//□□□
		//□□□
		//□■□
		if (y + 1 <= 30)
		{
			if (g_Grid[y + 1][x] != eBLOCKED)
			{
				if (g_Grid[y + 1][x] == eOPEN || g_Grid[y + 1][x] == eCLOSE)
				{
					//노드는 만들지 않는데, 여기 해당하는 노드의 F값을 비교해서 더 빠른길이면 새로 넣어주기
				}
				else if ((g_Grid[y + 1][x] == eBLANK) || (g_Grid[y + 1][x] == eEND))
				{
					NODE* newNode = new NODE(x, y + 1, popNode);
					setG(newNode);
					setH(newNode);
					setF(newNode);
					openList.push_back(newNode);
					g_Grid[y + 1][x] = eOPEN;
				}
			}
		}
		//□□□
		//□□□
		//□□■
		if (x + 1 <= 50 && y + 1 >= 30)
		{
			if (g_Grid[y + 1][x + 1] != eBLOCKED)
			{
				if (g_Grid[y + 1][x + 1] == eOPEN || g_Grid[y + 1][x + 1] == eCLOSE)
				{
					//노드는 만들지 않는데, 여기 해당하는 노드의 F값을 비교해서 더 빠른길이면 새로 넣어주기
				}
				else if ((g_Grid[y + 1][x + 1] == eBLANK) || (g_Grid[y + 1][x + 1] == eEND))
				{
					NODE* newNode = new NODE(x + 1, y + 1, popNode);
					setG_dia(newNode);
					setH(newNode);
					setF(newNode);
					openList.push_back(newNode);
					g_Grid[y + 1][x + 1] = eOPEN;
				}
			}
		}



	}
	InvalidateRect(hWnd, NULL, false);
}

void CAStar::pathDraw(HWND hWnd)
{
	if (endNode == nullptr)
		return;

	HDC hdc = GetDC(hWnd);


	NODE* temp = endNode;
	while (true)
	{
		HPEN Pen, oPen;
		Pen = CreatePen(PS_SOLID	, 3, RGB(255, 0, 0));
		oPen = (HPEN)SelectObject(hdc, Pen);
		MoveToEx(hdc, temp->pParent->ix* 20 + 10, temp->pParent->iy * 20+10,NULL);
		LineTo(hdc, temp->pParent->pParent->ix* 20 + 10, temp->pParent->pParent->iy* 20 + 10);
		SelectObject(hdc, oPen);
		DeleteObject(Pen);

		temp = temp->pParent;

		if (temp->pParent->pParent == nullptr)
			return;
	}

	InvalidateRect(hWnd, NULL, false);
	ReleaseDC(hWnd, hdc);
}

void CAStar::setEndNodeNULL()
{
	endNode = nullptr;
}

bool CAStar::isEndNode()
{
	if (endNode == nullptr)
		return false;
	else
		return true;
}
