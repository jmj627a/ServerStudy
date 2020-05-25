#pragma once

struct POS
{
	int m_ix;
	int m_iy;

	POS(int _x, int _y)
	{
		m_ix = _x;
		m_iy = _y;
	}
	POS() {}
};


class CBresenham
{
public:

	POS startPos;
	POS endPos;

	int addNum;
	int error;

public:
	CBresenham();
	~CBresenham();

	std::list<POS> dotList;

	void checkDot();
	
	bool compareLength(); //0-���α�/1-���α�

	void LineDraw(HWND hWnd);
};

