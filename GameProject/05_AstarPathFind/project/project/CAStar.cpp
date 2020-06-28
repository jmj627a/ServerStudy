#pragma once
#define _CRT_SECURE_NO_WARNINGS
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

void CAStar::init_map()
{
	char data;
	FILE* fp;
	auto err = fopen_s(&fp, "mapData.txt", "rb");

	int y = 0;
	int x = 0;

	while (fscanf_s(fp, "%c", &data) != EOF) {
		//printf("%c", data);
		switch (data)
		{
		case '0':
			g_Grid[y][x] = eBLANK;
			x++;
			if (x >= 50)
			{
				x = 0;
				y++;
			}
			break;
		case '1':
			g_Grid[y][x] = eSTART;
			x++;
			if (x >= 50)
			{
				x = 0;
				y++;
			}
			break;
		case '2':
			g_Grid[y][x] = eEND;
			x++;
			if (x >= 50)
			{
				x = 0;
				y++;
			}
			break;
		case '3':
			g_Grid[y][x] = eBLOCKED;
			x++;
			if (x >= 50)
			{
				x = 0;
				y++;
			}
			break;
		default:
			break;
		}

		if (y >=30 )
			break;
	}


	//openlist에 시작 노드 추가
	NODE* newNode = nullptr;

	for (int i = 0; i < 50; ++i)
		for (int j = 0; j < 30; ++j)
			if (g_Grid[j][i] == eSTART)
			{
				newNode = new NODE(i, j, nullptr);
				istartX = i;
				istartY = j;
			}

	for (int i = 0; i < 50; ++i)
		for (int j = 0; j < 30; ++j)
			if (g_Grid[j][i] == eEND)
			{
				setEndPos(i, j);
				setG(newNode);
				setH(newNode);
				setF(newNode);
				openList.push_back(newNode);
			}
}

bool CAStar::compareG(NODE* _node, bool isDia, int dir)
{
	std::list<NODE*>::iterator iter;

	int x = _node->ix;
	int y = _node->iy;

	switch (dir)
	{
	case 1:		
		x = x - 1;
		y = y - 1;
		break;
	case 2:
		y = y - 1;
		break;
	case 3:
		x = x + 1;
		y = y - 1;
		break;
	case 4:
		x = x - 1;
		break;
	case 5:
		x = x + 1;
		break;
	case 6:
		x = x - 1;
		y = y + 1;
		break;
	case 7:
		y = y + 1;
		break;
	case 8:
		x = x + 1;
		y = y + 1;
		break;
	}




	for (iter = openList.begin(); iter != openList.end(); ++iter)
	{
		if ((*iter)->iy == y && (*iter)->ix == x)
		{
			if (isDia)
			{
				if ((*iter)->G >= _node->G + 1.5)
				{
					(*iter)->pParent = _node;
					(*iter)->G = _node->G + 1.5;
					(*iter)->F = (*iter)->G + (*iter)->H;
					g_Grid[(*iter)->iy][(*iter)->ix] = eOPEN;
				}

				return true;
			}
			else
			{
				if ((*iter)->G >= _node->G + 1)
				{
					(*iter)->pParent = _node;
					(*iter)->G = _node->G + 1;
					(*iter)->F = (*iter)->G + (*iter)->H;
					g_Grid[(*iter)->iy][(*iter)->ix] = eOPEN;
				}
				return true;
			}
		}
	}

	return false;
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
	//단계별 출력때문에 timer에서 매번 호출
	//while (true)
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
			g_Grid[iendY][iendX] = eEND;
			g_Grid[istartY][istartX] = eSTART;

			//단계별 출력때문에 마지막 한번 더 호출 그냥 없어도 됨
			InvalidateRect(hWnd, NULL, false);
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
					compareG(popNode, true,1); //new노드랑, list 싹찾아서 비교
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
					compareG(popNode, false,2); //new노드랑, list 싹찾아서 비교
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
		if (x + 1 < 50 && y - 1 >= 0)
		{
			if (g_Grid[y - 1][x + 1] != eBLOCKED)
			{
				if (g_Grid[y - 1][x + 1] == eOPEN || g_Grid[y - 1][x + 1] == eCLOSE)
				{
					//노드는 만들지 않는데, 여기 해당하는 노드의 F값을 비교해서 더 빠른길이면 새로 넣어주기
					compareG(popNode, true,3); //new노드랑, list 싹찾아서 비교
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
					compareG(popNode, false,4); //new노드랑, list 싹찾아서 비교
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
					compareG(popNode, false,5); //new노드랑, list 싹찾아서 비교
					
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
		if (x - 1 >= 0 && y + 1 < 30)
		{
			if (g_Grid[y + 1][x - 1] != eBLOCKED)
			{
				if (g_Grid[y + 1][x - 1] == eOPEN || g_Grid[y + 1][x - 1] == eCLOSE)
				{
					//노드는 만들지 않는데, 여기 해당하는 노드의 F값을 비교해서 더 빠른길이면 새로 넣어주기
					compareG(popNode, true,6); //new노드랑, list 싹찾아서 비교
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
		if (y + 1 < 30)
		{
			if (g_Grid[y + 1][x] != eBLOCKED)
			{
				if (g_Grid[y + 1][x] == eOPEN || g_Grid[y + 1][x] == eCLOSE)
				{
					//노드는 만들지 않는데, 여기 해당하는 노드의 F값을 비교해서 더 빠른길이면 새로 넣어주기
					compareG(popNode, false,7); //new노드랑, list 싹찾아서 비교
					
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
		if (x + 1 < 50 && y + 1 < 30)
		{
			if (g_Grid[y + 1][x + 1] != eBLOCKED)
			{
				if (g_Grid[y + 1][x + 1] == eOPEN || g_Grid[y + 1][x + 1] == eCLOSE)
				{
					//노드는 만들지 않는데, 여기 해당하는 노드의 F값을 비교해서 더 빠른길이면 새로 넣어주기
					compareG(popNode, true,8); //new노드랑, list 싹찾아서 비교
					
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
	g_Grid[istartY][istartX] = eSTART;
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
