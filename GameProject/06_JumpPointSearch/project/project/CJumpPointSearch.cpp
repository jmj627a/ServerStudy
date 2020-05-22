#include "pch.h"
#include "CJumpPointSearch.h"

extern char g_Grid[WORLD_HEIGHT][WORLD_WIDTH];

bool CJumpPointSearch::checkWalkAble(int _x, int _y)
{
	if(g_Grid[_y][_x] == eBLOCKED)
		return false;

	return true;
}

void CJumpPointSearch::checkDirection(NODE* _pNode, int _x, int _y, int dir)
{
	int jumpX;
	int jumpY;

	switch (dir)
	{
		//��
	case eDIR_UL:
		findJumpNode(_x - 1, _y - 1, jumpX, jumpY);
		break;

		//��
	case eDIR_UU:
		break;

		//��
	case eDIR_UR:
		break;

		//��
	case eDIR_LL:
		break;

		//��
	case eDIR_RR:
		break;

		//��
	case eDIR_DL:
		break;

		//��
	case eDIR_DD:
		break;

		//��
	case eDIR_DR:
		break;
	}

}

bool CJumpPointSearch::findJumpNode(int _x, int _y, int& _jumpX, int& _jumpY)
{
	return false;
}


bool FComp(const NODE* lhs, const NODE* rhs) {
	return lhs->F < rhs->F;
};

int setDir(int _dirX, int _dirY)
{
	// 0����-����  0=����  0�̻�-������
	// 0����-����  0=����  0�̻�-�Ʒ���

	if (_dirX < 0 && _dirY < 0)		return eDIR_UL;
	if (_dirX == 0 && _dirY < 0)	return eDIR_UU;
	if (_dirX > 0 && _dirY < 0)		return eDIR_UR;
	if (_dirX < 0 && _dirY == 0)	return eDIR_RR;
	if (_dirX > 0 && _dirY == 0)	return eDIR_LL;
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
		g_Grid[m_iEndY][m_iEndX] = eEND;
		g_Grid[m_iStartY][m_iStartX] = eSTART;

		//�ܰ躰 ��¶����� ������ �ѹ� �� ȣ�� �׳� ��� ��
		InvalidateRect(hWnd, NULL, false);
		return;
	}

	closeList.push_back(popNode);
	int x = popNode->ix;
	int y = popNode->iy;
	g_Grid[y][x] = eCLOSE;


	//ù ���� 8���� Ž��
	if (popNode->pParent == nullptr)
	{
		checkDirection(popNode, x - 1, y - 1, eDIR_UL); //�� eDIR_UL
		checkDirection(popNode, x + 0, y - 1, eDIR_UU); //�� eDIR_UU
		checkDirection(popNode, x + 1, y - 1, eDIR_UR); //�� eDIR_UR
		checkDirection(popNode, x - 1, y + 0, eDIR_RR); //�� eDIR_RR
		checkDirection(popNode, x + 1, y + 0, eDIR_LL); //�� eDIR_LL
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
			if (g_Grid[y + 1][x] == eBLOCKED && g_Grid[y + 1][x - 1] == eBLANK)
				checkDirection(popNode, x - 1, y + 1, eDIR_DL);

			//���������� �ڳ�
			if (g_Grid[y][x + 1] == eBLOCKED && g_Grid[y - 1][x + 1] == eBLANK)
				checkDirection(popNode, x + 1, y - 1, eDIR_UR);
			break;

			//��
		case eDIR_UU:
			//�⺻ 1����
			checkDirection(popNode, x, y - 1, eDIR_UU);

			//�������� �ڳ�
			if (g_Grid[y][x-1] == eBLOCKED && g_Grid[y - 1][x - 1] == eBLANK)
				checkDirection(popNode, x - 1, y - 1, eDIR_UL);

			//���������� �ڳ�
			if (g_Grid[y][x + 1] == eBLOCKED && g_Grid[y - 1][x + 1] == eBLANK)
				checkDirection(popNode, x + 1, y - 1, eDIR_UR);
			break;

			//��
		case eDIR_UR:
			//�⺻ 3����
			checkDirection(popNode, x, y-1, eDIR_UU);
			checkDirection(popNode, x + 1, y - 1, eDIR_UR);
			checkDirection(popNode, x + 1, y, eDIR_RR);

			//�������� �ڳ�
			if (g_Grid[y][x - 1] == eBLOCKED && g_Grid[y - 1][x - 1] == eBLANK)
				checkDirection(popNode, x - 1, y - 1, eDIR_UL);

			//�Ʒ��� �ڳ�
			if (g_Grid[y + 1][x] == eBLOCKED && g_Grid[y + 1][x + 1] == eBLANK)
				checkDirection(popNode, x + 1, y + 1, eDIR_DR);
			break;

			//��
		case eDIR_LL :
			//�⺻ 1����
			checkDirection(popNode, x - 1 , y, eDIR_LL);

			//���� �ڳ�
			if (g_Grid[y-1][x] == eBLOCKED && g_Grid[y - 1][x - 1] == eBLANK)
				checkDirection(popNode, x - 1, y - 1, eDIR_UL);

			//�Ʒ��� �ڳ�
			if (g_Grid[y+1][x] == eBLOCKED && g_Grid[y + 1][x - 1] == eBLANK)
				checkDirection(popNode, x - 1, y + 1, eDIR_DL); 
			break;

			//��
		case eDIR_RR:
			//�⺻ 1����
			checkDirection(popNode, x+1, y , eDIR_RR);

			//���� �ڳ�
			if (g_Grid[y-1][x ] == eBLOCKED && g_Grid[y - 1][x + 1] == eBLANK)
				checkDirection(popNode, x + 1, y - 1, eDIR_UR);

			//�Ʒ��� �ڳ�
			if (g_Grid[y+1][x] == eBLOCKED && g_Grid[y + 1][x + 1] == eBLANK)
				checkDirection(popNode, x + 1, y + 1, eDIR_DR);
			break;

			//��
		case eDIR_DL:
			//�⺻ 3����
			checkDirection(popNode, x - 1, y, eDIR_LL);
			checkDirection(popNode, x - 1, y + 1, eDIR_DL);
			checkDirection(popNode, x, y + 1, eDIR_DD);

			//���� �ڳ�
			if (g_Grid[y - 1][x] == eBLOCKED && g_Grid[y - 1][x - 1] == eBLANK)
				checkDirection(popNode, x - 1, y - 1, eDIR_UL);

			//���������� �ڳ�
			if (g_Grid[y][x + 1] == eBLOCKED && g_Grid[y + 1][x + 1] == eBLANK)
				checkDirection(popNode, x + 1, y + 1, eDIR_DR);
			break;

			//��
		case eDIR_DD:
			//�⺻ 1����
			checkDirection(popNode, x, y + 1, eDIR_DD);

			//�������� �ڳ�
			if (g_Grid[y][x - 1] == eBLOCKED && g_Grid[y + 1][x - 1] == eBLANK)
				checkDirection(popNode, x - 1, y - 1, eDIR_DL);

			//���������� �ڳ�
			if (g_Grid[y][x + 1] == eBLOCKED && g_Grid[y + 1][x + 1] == eBLANK)
				checkDirection(popNode, x + 1, y + 1, eDIR_DR); 
			break;

			//��
		case eDIR_DR:
			//�⺻ 3����
			checkDirection(popNode, x, y + 1, eDIR_DD);
			checkDirection(popNode, x + 1, y + 1, eDIR_DR);
			checkDirection(popNode, x + 1, y, eDIR_RR);

			//���� �ڳ�
			if (g_Grid[y - 1][x] == eBLOCKED && g_Grid[y - 1][x + 1] == eBLANK)
				checkDirection(popNode, x + 1, y - 1, eDIR_UR);

			//�������� �ڳ�
			if (g_Grid[y][x - 1] == eBLOCKED && g_Grid[y + 1][x - 1] == eBLANK)
				checkDirection(popNode, x - 1, y + 1, eDIR_DL);
			break;

		}

	}

}
