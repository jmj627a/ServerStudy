#pragma once
#pragma once


#define WM_NETWORK WM_USER+1
#define GET_X_LPARAM(lp)  ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)  ((int)(short)HIWORD(lp))

//ȭ�� �̵� ����
#define dfRANGE_MOVE_TOP	0
#define dfRANGE_MOVE_LEFT	0
#define dfRANGE_MOVE_RIGHT	1000 //6400
#define dfRANGE_MOVE_BOTTOM	1000 //6400

// ĳ���� �̵� �ӵ�
#define dfSPEED_PLAYER_X	6
#define dfSPEED_PLAYER_Y	4

// �̵� ����üũ ���� / X�� �Ǵ� Y������ 50 Pixel �̻� �����߻��� ����
#define dfERROR_RANGE		50

// ���ݹ��� / �� �κ��� ���������� ��� �ϹǷ� �׳� �����
#define dfATTACK1_RANGE_X	80
#define dfATTACK2_RANGE_X	90
#define dfATTACK3_RANGE_X	100
#define dfATTACK1_RANGE_Y	10
#define dfATTACK2_RANGE_Y	10
#define dfATTACK3_RANGE_Y	20

// ���� ������ / �� �κ��� ���������� ��� �ϹǷ� �׳� �����
#define dfATTACK1_DAMAGE		1
#define dfATTACK2_DAMAGE		2
#define dfATTACK3_DAMAGE		3


//�ִϸ��̼� ������ ������
#define dfDELAY_STAND	5
#define dfDELAY_MOVE	4
#define dfDELAY_ATTACK1	3
#define dfDELAY_ATTACK2	4
#define dfDELAY_ATTACK3	4
#define dfDELAY_EFFECT	3

//�÷��̾� 8���⿡ ���� define
#define dfACTION_MOVE_LL 0
#define dfACTION_MOVE_LU 1
#define dfACTION_MOVE_UU 2
#define dfACTION_MOVE_RU 3
#define dfACTION_MOVE_RR 4
#define dfACTION_MOVE_RD 5
#define dfACTION_MOVE_DD 6
#define dfACTION_MOVE_LD 7

#define dfACTION_ATTACK1 8
#define dfACTION_ATTACK2 9
#define dfACTION_ATTACK3 10

#define dfACTION_PUSH 11
#define dfACTION_STAND 12

//#define dfDIR_LEFT		0
//#define dfDIR_RIGHT		4