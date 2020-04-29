#pragma once
class CSpriteDib;

class CEffectObject : public CBaseObject
{
public:
	CEffectObject(int _objectID, int _X, int _Y, int _delay, int _start, int _end);
	~CEffectObject();

	DWORD Run();
	void Render(BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch);

};

