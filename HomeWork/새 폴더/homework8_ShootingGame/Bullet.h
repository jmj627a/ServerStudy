#pragma once
class Bullet
{
public :

	bool flag = false;
	bool type; // 0 - PLAYER_TYPE , 1 - ENEMY_TYPE

	int x;
	int y;

	//총알 초기화
	void bulletInit();

	//누구 총알인지, 몬스터면 몇번째 몬스터한테 나온 총알인지
	bool create_bullet(bool type, int num);

	//버퍼 위치에 총알 그리기
	void Draw_Bullet(char chSprite);

	//총알 이동 1프레임에 1칸씩
	void bullet_Move();

	//총알 , 벽
	bool collision_Bullet_Wall();

	//총알 , 적
	bool collision_Bullet_Enemy();

	//총알 , 플레이어
	bool collision_Bullet_Player();
};

