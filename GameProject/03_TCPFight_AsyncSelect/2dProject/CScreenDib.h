#pragma once
#include "framework.h"
#ifndef __SCREEN_DIB__
#define __SCREEN_DIB__

/*
dib�� ����� GDI�� ��ũ�� ����
�������� HDC�� DIB�� ����Ͽ� �׸��� ����ش�.
*/
class CScreenDib
{
public:
	//������ �ı���
	CScreenDib(int iWidth, int iHeight, int iColorBit);
	virtual ~CScreenDib();

protected:
	void CreateDibBuffer(int iWidth, int iHeight, int iColorBit);
	void ReleaseDibBuffer(void);

public:
	void Flip(HWND hWnd, int iX = 0, int iY = 0);
	
	BYTE* GetDibBuffer(void);
	int GetWidth(void);
	int GetHeight(void);
	int GetPitch(void);

protected:
	BITMAPINFO m_stDibInfo;
	BYTE* m_bypBuffer;

	int m_iWidth;
	int m_iHeight;
	int m_iPitch;		//4����Ʈ ���ĵ� �� ���� ����Ʈ ��
	int m_iColorBit;
	int m_iBufferSize;
};


#endif
