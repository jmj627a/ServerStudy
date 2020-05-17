#pragma once
#include "CAStar.h"
#include "framework.h"


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

		//■□□
		//□□□
		//□□□
		if (g_Grid[y - 1][x - 1] != eBLOCKED)
		{
			if (g_Grid[y - 1][x - 1] == eOPEN || g_Grid[y - 1][x - 1] == eCLOSE)
			{
				//노드는 만들지 않는데, 여기 해당하는 노드의 F값을 비교해서 더 빠른길이면 새로 넣어주기
			}
			else if (g_Grid[y - 1][x - 1] == eBLANK)
			{
				NODE* newNode = new NODE(x - 1, y - 1, popNode);
				setG_dia(newNode);
				setH(newNode);
				setF(newNode);
				openList.push_back(newNode);
				g_Grid[y - 1][x - 1] = eOPEN;
			}
		}
		//□■□
		//□□□
		//□□□
		else if (g_Grid[y - 1][x] != eBLOCKED)
		{
			if (g_Grid[y - 1][x] == eOPEN || g_Grid[y - 1][x] == eCLOSE)
			{
				//노드는 만들지 않는데, 여기 해당하는 노드의 F값을 비교해서 더 빠른길이면 새로 넣어주기
			}
			else if (g_Grid[y - 1][x] == eBLANK)
			{
				NODE* newNode = new NODE(x, y - 1, popNode);
				setG(newNode);
				setH(newNode);
				setF(newNode);
				openList.push_back(newNode);
				g_Grid[y - 1][x] = eOPEN;
			}
		}
		//□□■
		//□□□
		//□□□
		else if (g_Grid[y - 1][x + 1] != eBLOCKED)
		{
			if (g_Grid[y - 1][x + 1] == eOPEN || g_Grid[y - 1][x + 1] == eCLOSE)
			{
				//노드는 만들지 않는데, 여기 해당하는 노드의 F값을 비교해서 더 빠른길이면 새로 넣어주기
			}
			else if (g_Grid[y - 1][x + 1] == eBLANK)
			{
				NODE* newNode = new NODE(x + 1, y - 1, popNode);
				setG_dia(newNode);
				setH(newNode);
				setF(newNode);
				openList.push_back(newNode);
				g_Grid[y - 1][x + 1] = eOPEN;
			}
		}
		//□□□
		//■□□
		//□□□
		else if (g_Grid[y][x - 1] != eBLOCKED)
		{
			if (g_Grid[y][x - 1] == eOPEN || g_Grid[y][x - 1] == eCLOSE)
			{
				//노드는 만들지 않는데, 여기 해당하는 노드의 F값을 비교해서 더 빠른길이면 새로 넣어주기
			}
			else if (g_Grid[y][x - 1] == eBLANK)
			{
				NODE* newNode = new NODE(x - 1, y, popNode);
				setG(newNode);
				setH(newNode);
				setF(newNode);
				openList.push_back(newNode);
				g_Grid[y][x - 1] = eOPEN;
			}
		}
		//□□□
		//□□■
		//□□□
		else if (g_Grid[y][x + 1] != eBLOCKED)
		{
			if (g_Grid[y][x + 1] == eOPEN || g_Grid[y][x + 1] == eCLOSE)
			{
				//노드는 만들지 않는데, 여기 해당하는 노드의 F값을 비교해서 더 빠른길이면 새로 넣어주기
			}
			else if (g_Grid[y][x + 1] == eBLANK)
			{
				NODE* newNode = new NODE(x + 1, y, popNode);
				setG(newNode);
				setH(newNode);
				setF(newNode);
				openList.push_back(newNode);
				g_Grid[y][x + 1] = eOPEN;
			}
		}
		//□□□
		//□□□
		//■□□
		else if (g_Grid[y + 1][x - 1] != eBLOCKED)
		{
			if (g_Grid[y + 1][x - 1] == eOPEN || g_Grid[y + 1][x - 1] == eCLOSE)
			{
				//노드는 만들지 않는데, 여기 해당하는 노드의 F값을 비교해서 더 빠른길이면 새로 넣어주기
			}
			else if (g_Grid[y + 1][x - 1] == eBLANK)
			{
				NODE* newNode = new NODE(x - 1, y + 1, popNode);
				setG_dia(newNode);
				setH(newNode);
				setF(newNode);
				openList.push_back(newNode);
				g_Grid[y + 1][x - 1] = eOPEN;
			}
		}
		//□□□
		//□□□
		//□■□
		else if (g_Grid[y + 1][x] != eBLOCKED)
		{
			if (g_Grid[y + 1][x] == eOPEN || g_Grid[y + 1][x] == eCLOSE)
			{
				//노드는 만들지 않는데, 여기 해당하는 노드의 F값을 비교해서 더 빠른길이면 새로 넣어주기
			}
			else if (g_Grid[y + 1][x] == eBLANK)
			{
				NODE* newNode = new NODE(x, y + 1, popNode);
				setG(newNode);
				setH(newNode);
				setF(newNode);
				openList.push_back(newNode);
				g_Grid[y + 1][x] = eOPEN;
			}
		}
		//□□□
		//□□□
		//□□■
		else if (g_Grid[y + 1][x + 1] != eBLOCKED)
		{
			if (g_Grid[y + 1][x + 1] == eOPEN || g_Grid[y + 1][x + 1] == eCLOSE)
			{
				//노드는 만들지 않는데, 여기 해당하는 노드의 F값을 비교해서 더 빠른길이면 새로 넣어주기
			}
			else if (g_Grid[y + 1][x + 1] == eBLANK)
			{
				NODE* newNode = new NODE(x + 1, y + 1, popNode);
				setG_dia(newNode);
				setH(newNode);
				setF(newNode);
				openList.push_back(newNode);
				g_Grid[y + 1][x + 1] = eOPEN;
			}
		}


		InvalidateRect(hWnd, NULL, false);
	}

}