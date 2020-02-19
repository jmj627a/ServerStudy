#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "Console.h"

//OBJECT
#define PLAYER			0
#define ENEMY			1	
#define BULLET			2


//SCENE
#define TITLE			0
#define GAME_STAGE1			1
#define GAME_STAGE2			2
#define GAME_STAGE3			3
#define GAMEOVER		4
//#define LOADING 3


//message queue에 넣을 것
#define DUMMY			0
#define KEY_UP			1
#define KEY_DOWN		2
#define KEY_LEFT		3
#define KEY_RIGHT		4
#define KEY_SPACE		5
#define KEY_F12			6

#define ENTER_STAGE1	10
#define ENTER_STAGE2	11
#define ENTER_STAGE3	12

#define GAME_OVER		20


//max값
#define STAGE_NUM		3
#define ENEMY_NUM		100
#define	BULLET_NUM		100