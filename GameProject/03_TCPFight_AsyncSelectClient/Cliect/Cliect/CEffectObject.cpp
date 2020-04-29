#include "stdafx.h"
#include "CBaseObject.h"
#include "CEffectObject.h"
#include "CSpriteDib.h"

extern CSpriteDib g_cSprite;

CEffectObject::CEffectObject(int _objectID, int _X, int _Y, int _delay, int _start, int _end)
{
	SetObjectID(_objectID);
	SetPosition(_X, _Y);
	SetSprite(_start, _end, _delay);
}


CEffectObject::~CEffectObject()
{
}

DWORD CEffectObject::Run()
{
	NextFrame();
	if (IsEndFrame())
		return true;

	return false;
}

void CEffectObject::Render(BYTE * bypDest, int iDestWidth, int iDestHeight, int iDestPitch)
{
	g_cSprite.DrawSprite(GetSprite(), m_iCurX, m_iCurY, bypDest
		, iDestWidth, iDestHeight, iDestPitch);
}
