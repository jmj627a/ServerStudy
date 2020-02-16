#pragma once
class CBaseObject
{
public :
	CBaseObject() {};
	CBaseObject(int _objectType, int _X, int _Y, bool _flag)
	{
		objectType = _objectType;
		X = _X;
		Y = _Y;
		flag = _flag;
	}

	virtual ~CBaseObject() {};
	
	virtual bool Action(void) = 0;
	virtual bool Draw(void) = 0;

	int GetObjectType() {
		return objectType;
	}

	bool GetFlag()
	{
		return flag;
	}

protected:
	int X;
	int Y;
	bool flag;
	int objectType;
};

