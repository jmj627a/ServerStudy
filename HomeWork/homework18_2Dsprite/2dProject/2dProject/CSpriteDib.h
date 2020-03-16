#pragma once
#include <Windows.h>

//bmp 파일을 읽어서 스프라이트 관리 클래스 

#ifndef __SPRITE_DIB__
#define __SPRITE_DIB__

class CSpriteDib
{
public:
	//dib 스프라이트 구조체
	//스프라이트 이미자와 사이즈 정보를 가짐

	typedef struct st_SPRITE
	{
		BYTE* bypImage;				//스프라이트 이미지 포인터
		int iWidth;					//width
		int iHeight;				//height
		int iPitch;					//pitch

		int iCenterPointX;			//중점x
		int iCenterPointY;			//중점y
	}st_SPRITE;


	//생성자 파괴자
	CSpriteDib(int iMaxSprite, DWORD dwColorKey);
	virtual ~CSpriteDib();

	//bmp 파일을 읽어서 하나의 프레임으로 저장한다
	BOOL LoadDibSprite(int iSpriteIndex, const wchar_t* szFileName, int iCenterPointX, int iCenterPointY);

	//해당 스프라이트 삭제
	void ReleaseSprite(int iSpriteIndex);

	//특정 메모리 위치에 스프라이트를 출력한다 (color key, cliping 처리)
	void DrawSprite(int iSpriteIndex, int iDrawX, int iDrawY, BYTE* bypDest, int iDestWidth,
		int iDestHeight, int iDestPitch, int iDrawLen = 100);

	//특정 메모리 위치에 이미지를 출력한다 (클리핑 처리)
	void DrawImage(int iSpriteIndex, int iDrawX, int iDrawY, BYTE* bypDest, int iDestWidth,
		int iDestHeight, int iDestPitch, int iDrawLen = 100);

protected:

	//sprite 배열 정보
	int m_iMaxSprite;
	st_SPRITE* m_stpSprite;

	//투명 색으로 사용할 컬러
	DWORD m_dwColorKey;
};

#endif