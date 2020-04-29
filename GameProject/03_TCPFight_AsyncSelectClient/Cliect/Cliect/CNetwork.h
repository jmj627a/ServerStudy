#pragma once
#include "stdafx.h"
#include "CRingBuffer.h"
#include "CPlayerObject.h"

class CNetwork
{
public:
	CNetwork();
	~CNetwork();


	bool RecvEvent();
	bool SendEvent();

	void PacketProc(BYTE byPacketType, char* Packet);

	BOOL netPacketProc_CreateMyCharacter(char* pPacketBuffer);
	BOOL netPacketProc_CreateOtherCharacter(char* pPacketBuffer);
	BOOL netPacketProc_DeleteCharacter(char* pPacketBuffer);
	
	BOOL netPacketProc_MoveCharacter(char* pPacketBuffer);
	BOOL netPacketProc_StopCharacter(char* pPacketBuffer);

	BOOL SendPacket(stNETWORK_PACKET_HEADER *pHeader, char *pPacket);


	void mpMoveStart(stNETWORK_PACKET_HEADER *pHeader, stPACKET_CS_MOVE_START *pPacket, int _dir, short _x, short _y);
	void mpMoveStop(stNETWORK_PACKET_HEADER *pHeader, stPACKET_CS_MOVE_STOP *pPacket, bool _dir, short _x, short _y);

	void mpAttack1();
	void mpAttack2();
	void mpAttack3();

};
