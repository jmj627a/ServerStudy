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

	void PacketProc(BYTE byPacketType, CPacket* Packet);

	BOOL netPacketProc_CreateMyCharacter(CPacket* pPacketBuffer);
	BOOL netPacketProc_CreateOtherCharacter(CPacket* pPacketBuffer);
	BOOL netPacketProc_DeleteCharacter(CPacket* pPacketBuffer);
	
	BOOL netPacketProc_MoveCharacter(CPacket* pPacketBuffer);
	BOOL netPacketProc_StopCharacter(CPacket* pPacketBuffer);

	BOOL netPacketProc_Attack1(CPacket* pPacketBuffer);
	BOOL netPacketProc_Attack2(CPacket* pPacketBuffer);
	BOOL netPacketProc_Attack3(CPacket* pPacketBuffer);

	BOOL netPacketProc_Damage(CPacket* pPacketBuffer);

	BOOL SendPacket(stNETWORK_PACKET_HEADER *pHeader, CPacket *pPacket);


	void mpMoveStart(stNETWORK_PACKET_HEADER *pHeader, CPacket *pPacket, int _dir, short _x, short _y);
	void mpMoveStop(stNETWORK_PACKET_HEADER *pHeader, CPacket *pPacket, int _dir, short _x, short _y);

	void mpAttack1(stNETWORK_PACKET_HEADER *pHeader, CPacket  *pPacket, int _dir, short _x, short _y);
	void mpAttack2(stNETWORK_PACKET_HEADER *pHeader, CPacket  *pPacket, int _dir, short _x, short _y);
	void mpAttack3(stNETWORK_PACKET_HEADER *pHeader, CPacket  *pPacket, int _dir, short _x, short _y);

};

