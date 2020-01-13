#pragma once
class Player
{
public :
	int x = 10;
	int y = 10;

	int life = 3;


	void playerInit();
	void KeyInput();

	bool isPlayerDeath();
	void playerDeathCheck();

	void Draw_Player(int iX, int iY, char chSprite);
};

