#pragma once

class CPlayer;

class UI
{
	CPlayer* player;
public:
	UI(CPlayer* _player) : player(_player) {};
	~UI() {};


	void printUI();
	void deleteUI();
};

