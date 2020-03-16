#pragma once
#include <Windows.h>

//bmp ������ �о ��������Ʈ ���� Ŭ���� 

#ifndef __SPRITE_DIB__
#define __SPRITE_DIB__

class CSpriteDib
{
public:
	//dib ��������Ʈ ����ü
	//��������Ʈ �̹��ڿ� ������ ������ ����

	typedef struct st_SPRITE
	{
		BYTE* bypImage;				//��������Ʈ �̹��� ������
		int iWidth;					//width
		int iHeight;				//height
		int iPitch;					//pitch

		int iCenterPointX;			//����x
		int iCenterPointY;			//����y
	}st_SPRITE;


	//������ �ı���
	CSpriteDib(int iMaxSprite, DWORD dwColorKey);
	virtual ~CSpriteDib();

	//bmp ������ �о �ϳ��� ���������� �����Ѵ�
	BOOL LoadDibSprite(int iSpriteIndex, const wchar_t* szFileName, int iCenterPointX, int iCenterPointY);

	//�ش� ��������Ʈ ����
	void ReleaseSprite(int iSpriteIndex);

	//Ư�� �޸� ��ġ�� ��������Ʈ�� ����Ѵ� (color key, cliping ó��)
	void DrawSprite(int iSpriteIndex, int iDrawX, int iDrawY, BYTE* bypDest, int iDestWidth,
		int iDestHeight, int iDestPitch, int iDrawLen = 100);

	//Ư�� �޸� ��ġ�� �̹����� ����Ѵ� (Ŭ���� ó��)
	void DrawImage(int iSpriteIndex, int iDrawX, int iDrawY, BYTE* bypDest, int iDestWidth,
		int iDestHeight, int iDestPitch, int iDrawLen = 100);

protected:

	//sprite �迭 ����
	int m_iMaxSprite;
	st_SPRITE* m_stpSprite;

	//���� ������ ����� �÷�
	DWORD m_dwColorKey;
};

#endif