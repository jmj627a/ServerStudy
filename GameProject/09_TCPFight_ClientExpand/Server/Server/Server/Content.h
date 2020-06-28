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

//���� �ϳ��� ��ǥ ����
struct st_SECTOR_POS
{
	int iX;
	int iY;
};

//Ư�� ��ġ �ֺ��� 9�� ���� ����
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
	
	DWORD dwAction;		//���� �׼� ��ȣ
	DWORD dwActionTick;		//(�ɼ�)�׼��� ���� �ð�. ���巹Ŀ�� ����
	BYTE byDirection; //�¿� LL RR
	BYTE byMoveDirection; // 8���� �̵�

	short shX;
	short shY;
	short shActionX;		//(�ɼ�)�׼��� ����� ���� ��ǥ. ���巹Ŀ�׿�
	short shActionY;		

	st_SECTOR_POS CurSector;		//���� ����
	st_SECTOR_POS OldSector;		//���� ���� 

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