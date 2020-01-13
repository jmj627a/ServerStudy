#pragma once
class Scene
{
public:
	//타이틀 씬
	void Title_Scene();

	//게임씬
	void Game_Scene();

	//게임오버 씬
	void Gameover_Scene();

	//넘겨주는 씬으로 변경
	void Change_Scene(int type);



	void Title_Print();

	void Title_keyInput();
};

