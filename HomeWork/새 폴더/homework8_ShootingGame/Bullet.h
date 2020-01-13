#pragma once
class Bullet
{
public :

	bool flag = false;
	bool type; // 0 - PLAYER_TYPE , 1 - ENEMY_TYPE

	int x;
	int y;

	//�Ѿ� �ʱ�ȭ
	void bulletInit();

	//���� �Ѿ�����, ���͸� ���° �������� ���� �Ѿ�����
	bool create_bullet(bool type, int num);

	//���� ��ġ�� �Ѿ� �׸���
	void Draw_Bullet(char chSprite);

	//�Ѿ� �̵� 1�����ӿ� 1ĭ��
	void bullet_Move();

	//�Ѿ� , ��
	bool collision_Bullet_Wall();

	//�Ѿ� , ��
	bool collision_Bullet_Enemy();

	//�Ѿ� , �÷��̾�
	bool collision_Bullet_Player();
};

