#pragma once
struct NODE
{
	int ix;
	int iy;
	NODE* pParent;

	double	F;	//G+H
	double	G;	//�����
	int		H;	//�������� �Ÿ�


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
	//��ã�� �Լ�
	void pathFind(HWND hWnd);


private:
	//x,y��ǥ�� ĭ�� ����ĭ�ΰ�?
	bool checkWalkAble(int _x, int _y);

	//�� �������� �� Ž��. �ڳʰ� �ִ°�? �ƴϸ� �� �ָ� �ڳʰ� �ִ°�?
	void checkDirection(NODE* _pNode, int _x, int _y, int dir);

	//checkDirection ���� ��带 ã�� �Լ�
	bool findJumpNode(int _x, int _y, int& _jumpX, int& _jumpY);



};

