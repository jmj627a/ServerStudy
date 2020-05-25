#include "pch.h"
#include "CJumpPointSearch.h"

extern TILE g_Grid[WORLD_HEIGHT][WORLD_WIDTH];

void CJumpPointSearch::setEndPos(int x, int y)
{
	m_iEndX = x;
	m_iEndY = y;
}

bool CJumpPointSearch::checkWalkAble(int _x, int _y)
{
	if(g_Grid[_y][_x].grid_type == eBLOCKED)
		return false;

	return true;
}

void CJumpPointSearch::setG(NODE* _node)
{
	if (_node->pParent != nullptr)
		_node->G = _node->pParent->G + 1;
	else
		_node->G = 0;
}

void CJumpPointSearch::setG_dia(NODE * _node)
{
	if (_node->pParent != nullptr)
		_node->G = _node->pParent->G + 1.5;
	else
		_node->G = 0;
}

bool CJumpPointSearch::compareG(NODE* _node, int dir)
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
			if (dir % 2 ==0 )
			{
				if ((*iter)->G >= _node->G + 1.5)
				{
					(*iter)->pParent = _node;
					(*iter)->G = _node->G + 1.5;
					(*iter)->F = (*iter)->G + (*iter)->H;
					g_Grid[(*iter)->iy][(*iter)->ix].grid_type = eOPEN;
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
					g_Grid[(*iter)->iy][(*iter)->ix].grid_type = eOPEN;
				}
				return true;
			}
		}
	}

	return false;
}

void CJumpPointSearch::setH(NODE * _node)
{
	_node->H = abs(_node->ix - m_iEndX) + abs(_node->iy - m_iEndY);
}

void CJumpPointSearch::setF(NODE * _node)
{
	_node->F = _node->G + _node->H;
}

void CJumpPointSearch::checkDirection(NODE* _pNode, int _x, int _y, int _dir)
{
	int jumpX;
	int jumpY;

	//�����ؼ� ��带 ���� ��ġ jump�� ��ƿ���
	if(findJumpNode(_x, _y, _dir, jumpX, jumpY))
	{
		//�̹� open�Ǿ��ִٸ� g�� ��
		if (g_Grid[jumpY][jumpX].grid_type == eOPEN)
		{
			std::list<NODE*>::iterator iter;

			for (iter = openList.begin(); iter != openList.end(); ++iter)
			{
				if ((*iter)->iy == jumpY && (*iter)->ix == jumpX)
				{
					if (_dir % 2 == 0) // �밢��
					{
						if ((*iter)->G >= _pNode->G + 1.5)
						{
							(*iter)->pParent = _pNode;
							(*iter)->G = _pNode->G + 1.5;
							(*iter)->F = (*iter)->G + (*iter)->H;
							g_Grid[(*iter)->iy][(*iter)->ix].grid_type = eOPEN;
						}
					}
					else //����
					{
						if ((*iter)->G >= _pNode->G + 1.0)
						{
							(*iter)->pParent = _pNode;
							(*iter)->G = _pNode->G + 1.0;
							(*iter)->F = (*iter)->G + (*iter)->H;
							g_Grid[(*iter)->iy][(*iter)->ix].grid_type = eOPEN;
						}
					}
					break;
				}
			}
		}
		//���ٸ� ���� ��� �����
		else if (g_Grid[jumpY][jumpX].grid_type == eBLANK || g_Grid[jumpY][jumpX].grid_type == eEND)
		{
			NODE* newNode = new NODE(jumpX, jumpY, _pNode);
			if (_dir % 2 == 0)	setG_dia(newNode);
			else				setG(newNode);
			setH(newNode);
			setF(newNode);
			openList.push_back(newNode);
			g_Grid[jumpY][jumpX].grid_type = eOPEN;
		}
	}
	

}

bool CJumpPointSearch::findJumpNode(int _x, int _y, int _dir, int& _jumpX, int& _jumpY)
{
	if (_x < 0 || _x >= WORLD_WIDTH) return false;
	if (_y < 0 || _y >= WORLD_HEIGHT) return false;
	if (g_Grid[_y][_x].grid_type==eBLOCKED) return false;

	_jumpX = _x;
	_jumpY = _y;

	if (_x == m_iEndX && _y == m_iEndY) 
		return true;

	//�ڳʳ�, �� �ָ� �ڳʸ� ã�ԵǸ� ��ǥ�� jump�� ��Ƽ� return true;
	switch (_dir)
	{
		//��
	case eDIR_UL:
		if (_y + 1 < WORLD_HEIGHT && _x - 1 >= 0)
		{
			if (g_Grid[_y + 1][_x].grid_type == eBLOCKED && g_Grid[_y + 1][_x - 1].grid_type == eBLANK)
				return true;
		}

		if (_x + 1 < WORLD_WIDTH && _y - 1 >= 0)
		{
			if(g_Grid[_y][_x + 1].grid_type == eBLOCKED && g_Grid[_y - 1][_x + 1].grid_type == eBLANK)
				return true;
		}

		if (findJumpNode(_x, _y - 1, eDIR_UU, _jumpX, _jumpY))
		{
			_jumpX = _x;
			_jumpY = _y;
			return true;
		}

		if (findJumpNode(_x - 1, _y, eDIR_LL, _jumpX, _jumpY))
		{
			_jumpX = _x;
			_jumpY = _y;
			return true;
		}

		findJumpNode(_x - 1, _y - 1, _dir, _jumpX, _jumpY);
		break;

		//��
	case eDIR_UU:
		//o
		//x ��
		if (_x - 1 >= 0 && _y - 1 >= 0)
		{
			if ((g_Grid[_y][_x - 1].grid_type == eBLOCKED && g_Grid[_y - 1][_x - 1].grid_type == eBLANK))
				return true;
		}
		//   o
		//�� x 
		if (_x + 1 < WORLD_WIDTH && _y - 1 >= 0)
		{
			if (g_Grid[_y][_x + 1].grid_type == eBLOCKED && g_Grid[_y - 1][_x + 1].grid_type == eBLANK)
				return true;
		}

		//������� Ž��
		findJumpNode(_x, _y - 1, _dir, _jumpX, _jumpY);
		break;

		//��
	case eDIR_UR:

		//O
		//X��
		if (_x - 1 >= 0 && _y - 1 >= 0)
		{
			if (g_Grid[_y][_x - 1].grid_type == eBLOCKED && g_Grid[_y - 1][_x - 1].grid_type == eBLANK)
				return true;
		}
		// ��
		// X O		
		if (_y + 1 < WORLD_HEIGHT && _x + 1 < WORLD_WIDTH)
		{
			if(g_Grid[_y + 1][_x].grid_type == eBLOCKED && g_Grid[_y + 1][_x + 1].grid_type == eBLANK)
				return true;
		}

		//���� Ž��
		if (findJumpNode(_x, _y - 1, eDIR_UU, _jumpX, _jumpY))
		{
			//���ָ� �ڳʸ� �߰��ؼ� ��ǥ�� �����Ծ ���� �� �ڸ����ٰ� ������ �ϱ⶧���� �� ��ǥ �־ ����
			_jumpX = _x;
			_jumpY = _y;
			return true;
		}

		//������ Ž��
		if (findJumpNode(_x + 1, _y, eDIR_RR, _jumpX, _jumpY))
		{
			//���ָ� �ڳʸ� �߰��ؼ� ��ǥ�� �����Ծ ���� �� �ڸ����ٰ� ������ �ϱ⶧���� �� ��ǥ �־ ����
			_jumpX = _x;
			_jumpY = _y; 
			return true;
		}

		//�ڱ� ����������� Ž��
		findJumpNode(_x + 1, _y - 1, _dir, _jumpX, _jumpY);
		break;

		//��
	case eDIR_LL:
		//o x
		//  ��
		if (_y - 1 >= 0 && _x - 1 >= 0)
		{
			if (g_Grid[_y - 1][_x].grid_type == eBLOCKED && g_Grid[_y - 1][_x - 1].grid_type == eBLANK)
				return true;
		}
		//  ��
		//o x
		if (_y + 1 < WORLD_HEIGHT && _x - 1 >= 0)
		{
			if (g_Grid[_y + 1][_x].grid_type == eBLOCKED && g_Grid[_y + 1][_x - 1].grid_type == eBLANK)
				return true;
		}
		
		//�ڱ� ����������� Ž��
		findJumpNode(_x - 1, _y, _dir, _jumpX, _jumpY);
		break;

		//��
	case eDIR_RR:
		if (_y + 1 < WORLD_HEIGHT && _x + 1 < WORLD_WIDTH)
		{
			if (g_Grid[_y + 1][_x].grid_type == eBLOCKED && g_Grid[_y + 1][_x + 1].grid_type == eBLANK)
				return true;
		}
		if (_y - 1 > 0 && _x + 1 <= WORLD_WIDTH)
		{
			if (g_Grid[_y - 1][_x].grid_type == eBLOCKED && g_Grid[_y - 1][_x + 1].grid_type == eBLANK)
				return true;
		}

		findJumpNode(_x + 1, _y, _dir, _jumpX, _jumpY);
		break;

		//��
	case eDIR_DL:
		if (_y - 1 >= 0 && _x - 1 >= 0)
		{
			if (g_Grid[_y - 1][_x].grid_type == eBLOCKED && g_Grid[_y - 1][_x - 1].grid_type == eBLANK)
				return true;
		}

		if (_x + 1 < WORLD_WIDTH && _y + 1 < WORLD_HEIGHT)
		{
			if (g_Grid[_y][_x + 1].grid_type == eBLOCKED && g_Grid[_y + 1][_x + 1].grid_type == eBLANK)
				return true;
		}

		if (findJumpNode(_x, _y + 1, eDIR_DD, _jumpX, _jumpY))
		{
			//���ָ� �ڳʸ� �߰��ؼ� ��ǥ�� �����Ծ ���� �� �ڸ����ٰ� ������ �ϱ⶧���� �� ��ǥ �־ ����
			_jumpX = _x;
			_jumpY = _y;
			return true;
		}

		if (findJumpNode(_x - 1, _y, eDIR_LL, _jumpX, _jumpY))
		{
			//���ָ� �ڳʸ� �߰��ؼ� ��ǥ�� �����Ծ ���� �� �ڸ����ٰ� ������ �ϱ⶧���� �� ��ǥ �־ ����
			_jumpX = _x;
			_jumpY = _y;
			return true;
		}

		findJumpNode(_x - 1, _y + 1, _dir, _jumpX, _jumpY);
		break;

		//��
	case eDIR_DD:
		if (_x - 1 >= 0 && _y + 1 < WORLD_HEIGHT)
		{
			if (g_Grid[_y][_x - 1].grid_type == eBLOCKED && g_Grid[_y + 1][_x - 1].grid_type == eBLANK)
				return true;
		}
		if (_x + 1 < WORLD_WIDTH && _y + 1 < WORLD_HEIGHT)
		{
			if(g_Grid[_y][_x + 1].grid_type == eBLOCKED && g_Grid[_y + 1][_x + 1].grid_type == eBLANK)
				return true;
		}

		findJumpNode(_x, _y + 1, _dir, _jumpX, _jumpY);
		break;

		//��
	case eDIR_DR:
		if (_y - 1 >=0  && _x + 1 < WORLD_WIDTH)
		{
			if (g_Grid[_y - 1][_x].grid_type == eBLOCKED && g_Grid[_y - 1][_x + 1].grid_type == eBLANK)
				return true;
		}
		if (_x - 1 >= 0 && _y + 1 < WORLD_HEIGHT)
		{
			if(g_Grid[_y][_x - 1].grid_type == eBLOCKED && g_Grid[_y + 1][_x - 1].grid_type == eBLANK)
				return true;
		}

		if (findJumpNode(_x, _y + 1, eDIR_DD, _jumpX, _jumpY))
		{
			//���ָ� �ڳʸ� �߰��ؼ� ��ǥ�� �����Ծ ���� �� �ڸ����ٰ� ������ �ϱ⶧���� �� ��ǥ �־ ����
			_jumpX = _x;
			_jumpY = _y;
			return true;
		}

		if (findJumpNode(_x + 1, _y, eDIR_RR, _jumpX, _jumpY))
		{
			//���ָ� �ڳʸ� �߰��ؼ� ��ǥ�� �����Ծ ���� �� �ڸ����ٰ� ������ �ϱ⶧���� �� ��ǥ �־ ����
			_jumpX = _x;
			_jumpY = _y;
			return true;
		}

		findJumpNode(_x + 1, _y + 1, _dir, _jumpX, _jumpY);
		break;
	}
}

void CJumpPointSearch::setEndNodeNULL()
{
	m_pEndNode = nullptr;
}

const bool FComp(const NODE* lhs, const NODE* rhs) {
	return lhs->F < rhs->F;
};

int setDir(int _dirX, int _dirY)
{
	// 0����-����  0=����  0�̻�-������
	// 0����-����  0=����  0�̻�-�Ʒ���

	if (_dirX < 0 && _dirY < 0)		return eDIR_UL;
	if (_dirX == 0 && _dirY < 0)	return eDIR_UU;
	if (_dirX > 0 && _dirY < 0)		return eDIR_UR;
	if (_dirX < 0 && _dirY == 0)	return eDIR_LL;
	if (_dirX > 0 && _dirY == 0)	return eDIR_RR;
	if (_dirX < 0 && _dirY > 0)		return eDIR_DL;
	if (_dirX == 0 && _dirY > 0)	return eDIR_DD;
	if (_dirX > 0 && _dirY > 0)		return eDIR_DR;

	return -999;
}

void CJumpPointSearch::pathFind(HWND hWnd)
{
	if (openList.size() == 0)
		return;

	openList.sort(FComp);
	NODE* popNode = openList.front();
	openList.pop_front();

	//��ã�� ��
	if (popNode->ix == m_iEndX && popNode->iy == m_iEndY)
	{
		m_pEndNode = popNode;
		g_Grid[m_iEndY][m_iEndX].grid_type = eEND;
		g_Grid[m_iStartY][m_iStartX].grid_type = eSTART;

		//�ܰ躰 ��¶����� ������ �ѹ� �� ȣ�� �׳� ��� ��
		InvalidateRect(hWnd, NULL, false);
		return;
	}

	closeList.push_back(popNode);
	int x = popNode->ix;
	int y = popNode->iy;
	g_Grid[y][x].grid_type = eCLOSE;


	//ù ���� 8���� Ž��
	if (popNode->pParent == nullptr)
	{
		checkDirection(popNode, x - 1, y - 1, eDIR_UL); //�� eDIR_UL
		checkDirection(popNode, x + 0, y - 1, eDIR_UU); //�� eDIR_UU
		checkDirection(popNode, x + 1, y - 1, eDIR_UR); //�� eDIR_UR
		checkDirection(popNode, x - 1, y + 0, eDIR_LL); //�� eDIR_LL
		checkDirection(popNode, x + 1, y + 0, eDIR_RR); //�� eDIR_RR
		checkDirection(popNode, x - 1, y + 1, eDIR_DL); //�� eDIR_DL
		checkDirection(popNode, x + 0, y + 1, eDIR_DD); //�� eDIR_DD
		checkDirection(popNode, x + 1, y + 1, eDIR_DR); //�� eDIR_DR
	}
	else
	{
		int dirX = popNode->ix - popNode->pParent->ix; // 0����-����  0=����  0�̻�-������
		int dirY = popNode->iy - popNode->pParent->iy; // 0����-����  0=����  0�̻�-�Ʒ��� 
		int dir;
		
		switch (setDir(dirX, dirY))
		{
			//��
		case eDIR_UL:
			//�⺻ 3����
			checkDirection(popNode, x - 1, y, eDIR_LL);
			checkDirection(popNode, x - 1, y - 1, eDIR_UL); 
			checkDirection(popNode, x , y - 1, eDIR_UU);

			//�Ʒ��� �ڳ�
			if (g_Grid[y + 1][x].grid_type == eBLOCKED && g_Grid[y + 1][x - 1].grid_type == eBLANK)
				checkDirection(popNode, x - 1, y + 1, eDIR_DL);

			//���������� �ڳ�
			if (g_Grid[y][x + 1].grid_type == eBLOCKED && g_Grid[y - 1][x + 1].grid_type == eBLANK)
				checkDirection(popNode, x + 1, y - 1, eDIR_UR);
			break;

			//��
		case eDIR_UU:
			//�⺻ 1����
			checkDirection(popNode, x, y - 1, eDIR_UU);

			//�������� �ڳ�
			if (g_Grid[y][x-1].grid_type == eBLOCKED && g_Grid[y - 1][x - 1].grid_type == eBLANK)
				checkDirection(popNode, x - 1, y - 1, eDIR_UL);

			//���������� �ڳ�
			if (g_Grid[y][x + 1].grid_type == eBLOCKED && g_Grid[y - 1][x + 1].grid_type == eBLANK)
				checkDirection(popNode, x + 1, y - 1, eDIR_UR);
			break;

			//��
		case eDIR_UR:
			//�⺻ 3����
			checkDirection(popNode, x, y-1, eDIR_UU);
			checkDirection(popNode, x + 1, y - 1, eDIR_UR);
			checkDirection(popNode, x + 1, y, eDIR_RR);

			//�������� �ڳ�
			if (g_Grid[y][x - 1].grid_type == eBLOCKED && g_Grid[y - 1][x - 1].grid_type == eBLANK)
				checkDirection(popNode, x - 1, y - 1, eDIR_UL);

			//�Ʒ��� �ڳ�
			if (g_Grid[y + 1][x].grid_type == eBLOCKED && g_Grid[y + 1][x + 1].grid_type == eBLANK)
				checkDirection(popNode, x + 1, y + 1, eDIR_DR);
			break;

			//��
		case eDIR_LL :
			//�⺻ 1����
			checkDirection(popNode, x - 1 , y, eDIR_LL);

			//���� �ڳ�
			if (g_Grid[y-1][x].grid_type == eBLOCKED && g_Grid[y - 1][x - 1].grid_type == eBLANK)
				checkDirection(popNode, x - 1, y - 1, eDIR_UL);

			//�Ʒ��� �ڳ�
			if (g_Grid[y+1][x].grid_type == eBLOCKED && g_Grid[y + 1][x - 1].grid_type == eBLANK)
				checkDirection(popNode, x - 1, y + 1, eDIR_DL); 
			break;

			//��
		case eDIR_RR:
			//�⺻ 1����
			checkDirection(popNode, x+1, y , eDIR_RR);

			//���� �ڳ�
			if (g_Grid[y-1][x ].grid_type == eBLOCKED && g_Grid[y - 1][x + 1].grid_type == eBLANK)
				checkDirection(popNode, x + 1, y - 1, eDIR_UR);

			//�Ʒ��� �ڳ�
			if (g_Grid[y+1][x].grid_type == eBLOCKED && g_Grid[y + 1][x + 1].grid_type == eBLANK)
				checkDirection(popNode, x + 1, y + 1, eDIR_DR);
			break;

			//��
		case eDIR_DL:
			//�⺻ 3����
			checkDirection(popNode, x - 1, y, eDIR_LL);
			checkDirection(popNode, x - 1, y + 1, eDIR_DL);
			checkDirection(popNode, x, y + 1, eDIR_DD);

			//���� �ڳ�
			if (g_Grid[y - 1][x].grid_type == eBLOCKED && g_Grid[y - 1][x - 1].grid_type == eBLANK)
				checkDirection(popNode, x - 1, y - 1, eDIR_UL);

			//���������� �ڳ�
			if (g_Grid[y][x + 1].grid_type == eBLOCKED && g_Grid[y + 1][x + 1].grid_type == eBLANK)
				checkDirection(popNode, x + 1, y + 1, eDIR_DR);
			break;

			//��
		case eDIR_DD:
			//�⺻ 1����
			checkDirection(popNode, x, y + 1, eDIR_DD);

			//�������� �ڳ�
			if (g_Grid[y][x - 1].grid_type == eBLOCKED && g_Grid[y + 1][x - 1].grid_type == eBLANK)
				checkDirection(popNode, x - 1, y + 1, eDIR_DL);

			//���������� �ڳ�
			if (g_Grid[y][x + 1].grid_type == eBLOCKED && g_Grid[y + 1][x + 1].grid_type == eBLANK)
				checkDirection(popNode, x + 1, y + 1, eDIR_DR); 
			break;

			//��
		case eDIR_DR:
			//�⺻ 3����
			checkDirection(popNode, x, y + 1, eDIR_DD);
			checkDirection(popNode, x + 1, y + 1, eDIR_DR);
			checkDirection(popNode, x + 1, y, eDIR_RR);

			//���� �ڳ�
			if (g_Grid[y - 1][x].grid_type == eBLOCKED && g_Grid[y - 1][x + 1].grid_type == eBLANK)
				checkDirection(popNode, x + 1, y - 1, eDIR_UR);

			//�������� �ڳ�
			if (g_Grid[y][x - 1].grid_type == eBLOCKED && g_Grid[y + 1][x - 1].grid_type == eBLANK)
				checkDirection(popNode, x - 1, y + 1, eDIR_DL);
			break;

		}

	}

	InvalidateRect(hWnd, NULL, false);

}


void CJumpPointSearch::pathDraw(HWND hWnd)
{
	if (m_pEndNode == nullptr)
		return;

	HDC hdc = GetDC(hWnd);


	NODE* temp = m_pEndNode;
	while (true)
	{
		HPEN Pen, oPen;
		Pen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
		oPen = (HPEN)SelectObject(hdc, Pen);
		MoveToEx(hdc, temp->ix * 20 + 10, temp->iy * 20 + 10, NULL);
		LineTo(hdc, temp->pParent->ix * 20 + 10, temp->pParent->iy * 20 + 10);
		SelectObject(hdc, oPen);
		DeleteObject(Pen);

		temp = temp->pParent;

		if (temp->pParent == nullptr)
			return;
	}

	InvalidateRect(hWnd, NULL, false);
	ReleaseDC(hWnd, hdc);
}