#pragma once
#include <Windows.h>

class CBaseObject
{
protected:
	bool m_bEndFrame;
	DWORD m_dwActionInput;
	
	int m_iCurX;
	int m_iCurY;
	
	int m_iDelayCount;
	int m_iFrameDelay;
	int m_iObjectID;		//ȸ����ȣ�� ������ �ο�

	int m_iSpriteEnd;
	int m_iSpriteNow;
	int m_iSpriteStart;
	int m_iSpriteMax;

public:
	CBaseObject();
	~CBaseObject();

	void ActionInput(DWORD dwAction);
	int GetCurX(void);
	int GetCurY(void) const ;
	int GetObjectID(void);
	int GetSprite(void);

	void SetSprite(int iSpriteStart, int iSpriteMax, int iFrameDelay);
	void SetPosition(int _CurX, int _CurY);
	void SetObjectID(int _ID); //���߿� �������� ���� �κ�

	int IsEndFrame(void);
	void NextFrame(void);

	virtual void Render(BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch);
	virtual DWORD Run();

};

