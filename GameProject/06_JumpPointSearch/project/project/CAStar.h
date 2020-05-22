#pragma once

struct NODE
{
	int ix;
	int iy;
	NODE *pParent;

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

class CAStar
{

public :
	int iendX;
	int iendY;
	int istartX;
	int istartY;
	NODE* endNode;
public:
	std::list<NODE*> openList;
	std::list<NODE*> closeList;


public:
	CAStar();
	~CAStar();


	//����
	void setG(NODE* _node);
	//�밢��
	void setG_dia(NODE* _node);
	bool compareG(NODE* _node, bool isDia, int dir);

	void setH(NODE* _node);
	void setF(NODE* _node);

	void setEndPos(int x, int y);

	void searchLoad(HWND hWnd);
	void pathDraw(HWND hWnd);

	void setEndNodeNULL();
	bool isEndNode();

};

