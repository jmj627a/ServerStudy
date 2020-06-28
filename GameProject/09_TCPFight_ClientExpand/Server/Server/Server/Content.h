#pragma once
#ifndef __CONTENT__
#define __CONTENT__

#include "protocol.h"
#include "ServerPrint.h"
#include "Network.h"
#include "global.h"

class CPacket;
class st_SESSION;

#define dfNETWORK_PACKET_RECV_TIMEOUT 1000000000

//섹터 하나의 좌표 정보
struct st_SECTOR_POS
{
	int iX;
	int iY;
};

//특정 위치 주변의 9개 섹터 정보
struct st_SECTOR_AROUND
{
	int iCount;
	st_SECTOR_POS Around[9];
};

struct st_CHARACTER
{
	SOCKET socket;
	st_SESSION *pSession;
	DWORD dwSessionID;
	
	DWORD dwAction;		//현재 액션 번호
	DWORD dwActionTick;		//(옵션)액션을 취한 시간. 데드레커닝 계산용
	BYTE byDirection; //좌우 LL RR
	BYTE byMoveDirection; // 8방향 이동

	short shX;
	short shY;
	short shActionX;		//(옵션)액션이 변경된 최초 좌표. 데드레커닝용
	short shActionY;		

	st_SECTOR_POS CurSector;		//현재 섹터
	st_SECTOR_POS OldSector;		//이전 섹터 

	st_SECTOR_AROUND Around;

	char chHP;
};

void Update(void);

st_CHARACTER* FindCharacter(DWORD _dwSessionID);

void CreateCharacter(st_SESSION *pSession);

void CharacterSectorUpdatePacket(st_CHARACTER* pCharacter);

bool netPacketProc_MoveStart(st_SESSION *pSession, CPacket *pPacket);
bool netPacketProc_MoveStop(st_SESSION *pSession, CPacket *pPacket);
bool netPacketProc_Attack1(st_SESSION *pSession, CPacket *pPacket);
bool netPacketProc_Attack2(st_SESSION *pSession, CPacket *pPacket);
bool netPacketProc_Attack3(st_SESSION *pSession, CPacket *pPacket);
bool netPacketProc_ECHO(st_SESSION *pSession, CPacket *pPacket);

void makePacket_CreateMyCharacter(st_CHARACTER* pCharacter, CPacket* pPacket);
void makePacket_CreateOtherCharacter(st_CHARACTER* pCharacter, CPacket* pPacket);
void makePacket_DeleteCharacter(st_CHARACTER* pCharacter, CPacket* pPacket);
void makePacket_MoveStart(st_CHARACTER* pCharacter, CPacket* pPacket);
void makePacket_MoveStop(st_CHARACTER* pCharacter, CPacket* pPacket);
void makePacket_Attack1(st_CHARACTER* pCharacter, CPacket* pPacket);
void makePacket_Attack2(st_CHARACTER* pCharacter, CPacket* pPacket);
void makePacket_Attack3(st_CHARACTER* pCharacter, CPacket* pPacket);
void makePacket_Damage(st_CHARACTER * pAttackCharacter, st_CHARACTER * pDamageCharacter, CPacket* pPacket);
void makePacket_Sync(st_CHARACTER* pCharacter, CPacket* pPacket);
void makePacket_ECHO(st_CHARACTER* pCharacter, CPacket* pPacket);


#endif