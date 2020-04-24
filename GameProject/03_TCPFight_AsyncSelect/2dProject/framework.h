﻿// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>
// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

enum e_OBJECT_TYPE
{
	eTYPE_PLAYER,
	eTYPE_EFFECT
};

enum e_SPRITE
{
	eMap,

	ePLAYER_STAND_L01,
	ePLAYER_STAND_L02,
	ePLAYER_STAND_L03,
	ePLAYER_STAND_L04,
	ePLAYER_STAND_L05,
	ePLAYER_STAND_L_MAX,

	ePLAYER_STAND_R01,
	ePLAYER_STAND_R02,
	ePLAYER_STAND_R03,
	ePLAYER_STAND_R04,
	ePLAYER_STAND_R05,
	ePLAYER_STAND_R_MAX,

	ePLAYER_MOVE_L01,
	ePLAYER_MOVE_L02,
	ePLAYER_MOVE_L03,
	ePLAYER_MOVE_L04,
	ePLAYER_MOVE_L05,
	ePLAYER_MOVE_L06,
	ePLAYER_MOVE_L07,
	ePLAYER_MOVE_L08,
	ePLAYER_MOVE_L09,
	ePLAYER_MOVE_L10,
	ePLAYER_MOVE_L11,
	ePLAYER_MOVE_L12,
	ePLAYER_MOVE_L_MAX,

	ePLAYER_MOVE_R01,
	ePLAYER_MOVE_R02,
	ePLAYER_MOVE_R03,
	ePLAYER_MOVE_R04,
	ePLAYER_MOVE_R05,
	ePLAYER_MOVE_R06,
	ePLAYER_MOVE_R07,
	ePLAYER_MOVE_R08,
	ePLAYER_MOVE_R09,
	ePLAYER_MOVE_R10,
	ePLAYER_MOVE_R11,
	ePLAYER_MOVE_R12,
	ePLAYER_MOVE_R_MAX,

	ePLAYER_ATTACK1_L01,
	ePLAYER_ATTACK1_L02,
	ePLAYER_ATTACK1_L03,
	ePLAYER_ATTACK1_L04,
	ePLAYER_ATTACK1_L_MAX,

	ePLAYER_ATTACK1_R01,
	ePLAYER_ATTACK1_R02,
	ePLAYER_ATTACK1_R03,
	ePLAYER_ATTACK1_R04,
	ePLAYER_ATTACK1_R_MAX,

	ePLAYER_ATTACK2_L01,
	ePLAYER_ATTACK2_L02,
	ePLAYER_ATTACK2_L03,
	ePLAYER_ATTACK2_L04,
	ePLAYER_ATTACK2_L_MAX,

	ePLAYER_ATTACK2_R01,
	ePLAYER_ATTACK2_R02,
	ePLAYER_ATTACK2_R03,
	ePLAYER_ATTACK2_R04,
	ePLAYER_ATTACK2_R_MAX,

	ePLAYER_ATTACK3_L01,
	ePLAYER_ATTACK3_L02,
	ePLAYER_ATTACK3_L03,
	ePLAYER_ATTACK3_L04,
	ePLAYER_ATTACK3_L05,
	ePLAYER_ATTACK3_L06,
	ePLAYER_ATTACK3_L_MAX,

	ePLAYER_ATTACK3_R01,
	ePLAYER_ATTACK3_R02,
	ePLAYER_ATTACK3_R03,
	ePLAYER_ATTACK3_R04,
	ePLAYER_ATTACK3_R05,
	ePLAYER_ATTACK3_R06,
	ePLAYER_ATTACK3_R_MAX,

	eEFFECT_SPARK_01,
	eEFFECT_SPARK_02,
	eEFFECT_SPARK_03,
	eEFFECT_SPARK_04,
	eEFFECT_SPARK_MAX,

	eGUAGE_HP,
	eSHADOW,
	eSPRITE_MAX
};

//화면 이동 영역
#define dfRANGE_MOVE_TOP	50
#define dfRANGE_MOVE_LEFT	10
#define dfRANGE_MOVE_RIGHT	630
#define dfRANGE_MOVE_BOTTOM	470

//애니메이션 프레임 딜레이
#define dfDELAY_STAND	5
#define dfDELAY_MOVE	4
#define dfDELAY_ATTACK1	3
#define dfDELAY_ATTACK2	4
#define dfDELAY_ATTACK3	4
#define dfDELAY_EFFECT	3

//플레이어 8방향에 대한 define
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

#define dfDIR_LEFT 0
#define dfDIR_RIGHT 1