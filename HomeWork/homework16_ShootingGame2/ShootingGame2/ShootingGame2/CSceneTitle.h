#pragma once
#include "CSceneBase.h"
#include "global.h"

struct Word
{
	int text[6][3];

	int startX;
	int startY;

	int x;
	int y;

	bool dir;
	int size = 6;
};

class CSceneTitle : public CSceneBase
{

private:
	Word word[4] = {
		{{0,1,0,
		 1,0,0,
		 1,1,1,
		 1,0,1,
		 1,0,1,
		 0,1,0},
		 2,2,2,2,0,6},

		{{0,1,0,
		 1,0,1,
		 1,1,1,
		 1,0,1,
		 1,0,1,
		 0,0,0},
		 6,5,6,5,0,6},

		{{1,0,1,
		 1,1,1,
		 1,1,1,
		 1,0,1,
		 1,0,1,
		 0,0,0},
		 10,9,10,9,0,6},

		{{1,1,1,
		 1,0,0,
		 1,1,1,
		 1,0,0,
		 1,1,1,
		 0,0,0},
		 14,12,14,12,0,6}
	};

	int chooseStage = 0;

public:

	CSceneTitle()
	{
	}

	void Title_Move();
	void Title_keyInput();
	void Title_Draw();
	void Title_Print();

	virtual void Update()
	{
		//키보드
		Title_keyInput();

		//로직
		Title_Move();
		
		//랜더
		Title_Draw();
		Title_Print();

	}
};

