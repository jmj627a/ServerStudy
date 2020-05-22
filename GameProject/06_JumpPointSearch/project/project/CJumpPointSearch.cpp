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
		//↖
	case eDIR_UL:
		findJumpNode(_x - 1, _y - 1, jumpX, jumpY);
		break;

		//↑
	case eDIR_UU:
		break;

		//↗
	case eDIR_UR:
		break;

		//←
	case eDIR_LL:
		break;

		//→
	case eDIR_RR:
		break;

		//↙
	case eDIR_DL:
		break;

		//↓
	case eDIR_DD:
		break;

		//↘
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
	// 0이하-왼쪽  0=같음  0이상-오른쪽
	// 0이하-위쪽  0=같음  0이상-아래쪽

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

	//길찾기 끝
	if (popNode->ix == m_iEndX && popNode->iy == m_iEndY)
	{
		m_pEndNode = popNode;
		g_Grid[m_iEndY][m_iEndX] = eEND;
		g_Grid[m_iStartY][m_iStartX] = eSTART;

		//단계별 출력때문에 마지막 한번 더 호출 그냥 없어도 됨
		InvalidateRect(hWnd, NULL, false);
		return;
	}

	closeList.push_back(popNode);
	int x = popNode->ix;
	int y = popNode->iy;
	g_Grid[y][x] = eCLOSE;


	//첫 노드면 8방향 탐색
	if (popNode->pParent == nullptr)
	{
		checkDirection(popNode, x - 1, y - 1, eDIR_UL); //↖ eDIR_UL
		checkDirection(popNode, x + 0, y - 1, eDIR_UU); //↑ eDIR_UU
		checkDirection(popNode, x + 1, y - 1, eDIR_UR); //↗ eDIR_UR
		checkDirection(popNode, x - 1, y + 0, eDIR_RR); //← eDIR_RR
		checkDirection(popNode, x + 1, y + 0, eDIR_LL); //→ eDIR_LL
		checkDirection(popNode, x - 1, y + 1, eDIR_DL); //↙ eDIR_DL
		checkDirection(popNode, x + 0, y + 1, eDIR_DD); //↓ eDIR_DD
		checkDirection(popNode, x + 1, y + 1, eDIR_DR); //↘ eDIR_DR
	}
	else
	{
		int dirX = popNode->ix - popNode->pParent->ix; // 0이하-왼쪽  0=같음  0이상-오른쪽
		int dirY = popNode->iy - popNode->pParent->iy; // 0이하-위쪽  0=같음  0이상-아래쪽 
		int dir;
		
		switch (setDir(dirX, dirY))
		{
			//↖
		case eDIR_UL:
			//기본 3방향
			checkDirection(popNode, x - 1, y, eDIR_LL);
			checkDirection(popNode, x - 1, y - 1, eDIR_UL); 
			checkDirection(popNode, x , y - 1, eDIR_UU);

			//아래로 코너
			if (g_Grid[y + 1][x] == eBLOCKED && g_Grid[y + 1][x - 1] == eBLANK)
				checkDirection(popNode, x - 1, y + 1, eDIR_DL);

			//오른쪽으로 코너
			if (g_Grid[y][x + 1] == eBLOCKED && g_Grid[y - 1][x + 1] == eBLANK)
				checkDirection(popNode, x + 1, y - 1, eDIR_UR);
			break;

			//↑
		case eDIR_UU:
			//기본 1방향
			checkDirection(popNode, x, y - 1, eDIR_UU);

			//왼쪽으로 코너
			if (g_Grid[y][x-1] == eBLOCKED && g_Grid[y - 1][x - 1] == eBLANK)
				checkDirection(popNode, x - 1, y - 1, eDIR_UL);

			//오른쪽으로 코너
			if (g_Grid[y][x + 1] == eBLOCKED && g_Grid[y - 1][x + 1] == eBLANK)
				checkDirection(popNode, x + 1, y - 1, eDIR_UR);
			break;

			//↗
		case eDIR_UR:
			//기본 3방향
			checkDirection(popNode, x, y-1, eDIR_UU);
			checkDirection(popNode, x + 1, y - 1, eDIR_UR);
			checkDirection(popNode, x + 1, y, eDIR_RR);

			//왼쪽으로 코너
			if (g_Grid[y][x - 1] == eBLOCKED && g_Grid[y - 1][x - 1] == eBLANK)
				checkDirection(popNode, x - 1, y - 1, eDIR_UL);

			//아래로 코너
			if (g_Grid[y + 1][x] == eBLOCKED && g_Grid[y + 1][x + 1] == eBLANK)
				checkDirection(popNode, x + 1, y + 1, eDIR_DR);
			break;

			//←
		case eDIR_LL :
			//기본 1방향
			checkDirection(popNode, x - 1 , y, eDIR_LL);

			//위로 코너
			if (g_Grid[y-1][x] == eBLOCKED && g_Grid[y - 1][x - 1] == eBLANK)
				checkDirection(popNode, x - 1, y - 1, eDIR_UL);

			//아래로 코너
			if (g_Grid[y+1][x] == eBLOCKED && g_Grid[y + 1][x - 1] == eBLANK)
				checkDirection(popNode, x - 1, y + 1, eDIR_DL); 
			break;

			//→
		case eDIR_RR:
			//기본 1방향
			checkDirection(popNode, x+1, y , eDIR_RR);

			//위로 코너
			if (g_Grid[y-1][x ] == eBLOCKED && g_Grid[y - 1][x + 1] == eBLANK)
				checkDirection(popNode, x + 1, y - 1, eDIR_UR);

			//아래로 코너
			if (g_Grid[y+1][x] == eBLOCKED && g_Grid[y + 1][x + 1] == eBLANK)
				checkDirection(popNode, x + 1, y + 1, eDIR_DR);
			break;

			//↙
		case eDIR_DL:
			//기본 3방향
			checkDirection(popNode, x - 1, y, eDIR_LL);
			checkDirection(popNode, x - 1, y + 1, eDIR_DL);
			checkDirection(popNode, x, y + 1, eDIR_DD);

			//위로 코너
			if (g_Grid[y - 1][x] == eBLOCKED && g_Grid[y - 1][x - 1] == eBLANK)
				checkDirection(popNode, x - 1, y - 1, eDIR_UL);

			//오른쪽으로 코너
			if (g_Grid[y][x + 1] == eBLOCKED && g_Grid[y + 1][x + 1] == eBLANK)
				checkDirection(popNode, x + 1, y + 1, eDIR_DR);
			break;

			//↓
		case eDIR_DD:
			//기본 1방향
			checkDirection(popNode, x, y + 1, eDIR_DD);

			//왼쪽으로 코너
			if (g_Grid[y][x - 1] == eBLOCKED && g_Grid[y + 1][x - 1] == eBLANK)
				checkDirection(popNode, x - 1, y - 1, eDIR_DL);

			//오른쪽으로 코너
			if (g_Grid[y][x + 1] == eBLOCKED && g_Grid[y + 1][x + 1] == eBLANK)
				checkDirection(popNode, x + 1, y + 1, eDIR_DR); 
			break;

			//↘
		case eDIR_DR:
			//기본 3방향
			checkDirection(popNode, x, y + 1, eDIR_DD);
			checkDirection(popNode, x + 1, y + 1, eDIR_DR);
			checkDirection(popNode, x + 1, y, eDIR_RR);

			//위로 코너
			if (g_Grid[y - 1][x] == eBLOCKED && g_Grid[y - 1][x + 1] == eBLANK)
				checkDirection(popNode, x + 1, y - 1, eDIR_UR);

			//왼쪽으로 코너
			if (g_Grid[y][x - 1] == eBLOCKED && g_Grid[y + 1][x - 1] == eBLANK)
				checkDirection(popNode, x - 1, y + 1, eDIR_DL);
			break;

		}

	}

}
