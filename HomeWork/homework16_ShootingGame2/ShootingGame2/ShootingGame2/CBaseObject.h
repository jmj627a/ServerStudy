#pragma once
class CBaseObject
{
public :
	CBaseObject() {};
	CBaseObject(int ObjectType, int X, int Y)
	{
		_ObjectType = ObjectType;
		_X = X;
		_Y = Y;
	}

	virtual ~CBaseObject();
	
	virtual bool Action(void) = 0;
	virtual bool Draw(void) = 0;

	int GetObjectType() {
		return _ObjectType;
	}


protected:
	int _X;
	int _Y;
	int _ObjectType;
};

