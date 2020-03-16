#pragma once
#include <Windows.h>

class CBaseObject
{
private:
	bool m_bEndFrame;
	DWORD m_dwActionInput;
	
	int m_iCurX;
	int m_iCurY;
	
	int m_iDelayCount;
	int m_iFrameDelay;
	int m_iObjectID;

	int m_iSpriteEnd;
	int m_iSpriteNow;
	int m_iSpriteStart;
	int m_iSpriteMax;

public:
	CBaseObject();
	~CBaseObject();

	void ActionInput(void);
	int GetCurX(void);
	int GetCurY(void);
	int GetObjectID(void);
	int GetSprite(void);

	void SetSprite(int iSpriteStart, int iSpriteMax, int iFrameDelay);
	void SetPosition(int _CurX, int _CurY);
	
	int IsEndFrame(void);
	void NextFrame(void);

	virtual void Render();
	virtual void Run();


};

