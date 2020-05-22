#pragma once
struct NODE
{
	int ix;
	int iy;
	NODE* pParent;

	double	F;	//G+H
	double	G;	//출발점
	int		H;	//목적지와 거리


	NODE(int x, int y, NODE* parent)
	{
		ix = x;
		iy = y;
		pParent = parent;
	}
};


class CJumpPointSearch
{
public:
	std::list<NODE*> openList;
	std::list<NODE*> closeList;

	NODE* m_pEndNode;

	int m_iStartX;
	int m_iStartY;

	int m_iEndX;
	int m_iEndY;

public:
	//길찾기 함수
	void pathFind(HWND hWnd);


private:
	//x,y좌표의 칸이 막힌칸인가?
	bool checkWalkAble(int _x, int _y);

	//이 방향으로 쭉 탐색. 코너가 있는가? 아니면 저 멀리 코너가 있는가?
	void checkDirection(NODE* _pNode, int _x, int _y, int dir);

	//checkDirection 에서 노드를 찾는 함수
	bool findJumpNode(int _x, int _y, int& _jumpX, int& _jumpY);



};

