#include <iostream>


class CPlayer
{
	int _a;
	int _b;
	char _c;
	float _casdf;
	int _csd;
	int _dfs;
	int _cssdd;
	int _csddd;
	int _cssdfsd;
	int _csssdd;

public:
	CPlayer(int a, int b) : _a(a), _b(b) {}
};
void main()
{
	CPlayer player1(1, 2);
	CPlayer player2(player1);
}