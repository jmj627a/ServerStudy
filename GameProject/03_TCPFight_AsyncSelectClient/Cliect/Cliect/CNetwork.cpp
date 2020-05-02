#pragma once
#include "stdafx.h"
#include "CNetwork.h"
#include "CEffectObject.h"

extern CRingBuffer g_RecvBuffer;
extern CRingBuffer g_SendBuffer;

extern bool g_SendPossible;
extern SOCKET Socket;

extern list<CBaseObject*> g_objectList;
extern CPlayerObject* g_pPlayerObject;

CNetwork::CNetwork()
{
}


CNetwork::~CNetwork()
{
}

bool CNetwork::RecvEvent()
{
	char recvBuffer[1000];
	int ret = recv(Socket, recvBuffer, 1000, 0);
	if (ret == SOCKET_ERROR)
		return false;

	char* pbuf = recvBuffer;

	//recv�Ѱ� �� enq �� ������. �̰� �ٷ� �����ۿ� recv�ϸ� ����� ����
	//while (ret != 0)
	{
		ret = g_RecvBuffer.Enqueue(pbuf, ret);

		stNETWORK_PACKET_HEADER Header;

		//�Ϸ� ��Ŷ ó����
		while (true)
		{
			//1. RecvQ�� �ּ����� ����� �ִ��� Ȯ��. ����� ���̺��� ������ ���� ���Դ� �Ǵ� �� ������
			if (g_RecvBuffer.GetUseSize() <= sizeof(stNETWORK_PACKET_HEADER)) break;

			//2. RecvQ���� ����� Peek
			ret = g_RecvBuffer.Peek((char*)&Header, sizeof(stNETWORK_PACKET_HEADER));

			//3. ����� code Ȯ��
			if (Header.byCode != dfNETWORK_PACKET_CODE) return false;

			//4. ����� len ���� RecvQ�� ������ ������ �� (��� + len + �����ڵ�)  ��Ŷ �ϳ��� �ϼ��� �ȵ�����(����� ��� ��Ŷ ���� + ��� ũ��) �� �� ������ ������
			if (g_RecvBuffer.GetUseSize() < sizeof(Header) + Header.bySize + sizeof(BYTE)) return false;

			//��� ������ ��Ŷ�� ����
			g_RecvBuffer.MoveFront(sizeof(Header));

			//char* Packet = (char*)malloc(Header.bySize);
			CPacket Packet;

			ret = g_RecvBuffer.Dequeue((char*)Packet.GetBufferPtr(), Header.bySize);

			BYTE endCode;
			g_RecvBuffer.Peek((char*)&endCode, 1);
			if (endCode != dfNETWORK_PACKET_END) return false;
			g_RecvBuffer.MoveFront(sizeof(endCode));

			PacketProc(Header.byType, &Packet);
		}
	}
}

//FD_WRITE�� ���� ��. �� �Լ��� sendQueue.GetUseSize() <=0 �� �� ���� while�� ������ �Ѵ�. �ƴϸ� ���� �� �� ����.
bool CNetwork::SendEvent()
{
	if (!g_SendPossible)
		return false;

	//ť�� �ִ� �� �����ֱ� 
	while (true)
	{
		char sendBuffer[1000]; //�̰� ���ļ� send �ϴµ� ���⼱ peek�� Ȯ�� 
		int ret = g_SendBuffer.Peek(sendBuffer, 1000);

		ret = send(Socket, sendBuffer, ret, 0);

		if (ret == SOCKET_ERROR)
		{
			int err = WSAGetLastError();

			//������ �ƴ϶�� 
			if (err != WSAEWOULDBLOCK)
				return false;
			//������ �´ٸ� -> �ƹ��͵� ī�Ǹ� ���� ��. FD_WRITE�� �߱� ������ ����, �� Ȯ���� �ʿ� ����. isSend=false�� �ٲٰ� �׳� ������. ���߿� FD_WRITE���� true�� �ٲ��ٰŴ�. 
			else
			{
				g_SendPossible = false;
				return false;
			}

		}

		//������ ���� �ʾ����� ���� ���ٴ� ��. sendQueue���� removeFront(ret)�� copy�� ������ ret��ŭ�� ������
		g_SendBuffer.MoveFront(ret);

		//session->SendQ �� ��� ������ �ݺ��� �ߴ�
		if (g_SendBuffer.GetUseSize() == 0)
			return true;
	}
}


void CNetwork::PacketProc(BYTE byPacketType, CPacket* Packet)
{
	switch (byPacketType)
	{
	case dfPACKET_SC_CREATE_MY_CHARACTER:
		netPacketProc_CreateMyCharacter(Packet); 
		break;

	case dfPACKET_SC_CREATE_OTHER_CHARACTER:
		netPacketProc_CreateOtherCharacter(Packet); 
		break;

	case dfPACKET_SC_DELETE_CHARACTER:
		netPacketProc_DeleteCharacter(Packet);
		break;

	case dfPACKET_SC_MOVE_START:
		netPacketProc_MoveCharacter(Packet);
		break;

	case dfPACKET_SC_MOVE_STOP:
		netPacketProc_StopCharacter(Packet);
		break;

	case dfPACKET_SC_ATTACK1:
		netPacketProc_Attack1(Packet);
		break;

	case dfPACKET_SC_ATTACK2:
		netPacketProc_Attack2(Packet);
		break;

	case dfPACKET_SC_ATTACK3:
		netPacketProc_Attack3(Packet);
		break;

	case dfPACKET_SC_DAMAGE:
		netPacketProc_Damage(Packet);
		break;

		//Ŭ�󿡼� ó���ؾ��� ��Ŷ���� ó�� �Լ� ȣ��
	}

}

BOOL CNetwork::netPacketProc_CreateMyCharacter(CPacket *pPacketBuffer)
{
	//stPACKET_SC_CREATE_MY_CHARACTER * pPacket = (stPACKET_SC_CREATE_MY_CHARACTER*)pPacketBuffer;
	DWORD ID;
	BYTE Direction;
	WORD X;
	WORD Y;
	BYTE HP;

	*pPacketBuffer >> ID;
	*pPacketBuffer >> Direction;
	*pPacketBuffer >> X;
	*pPacketBuffer >> Y;
	*pPacketBuffer >> HP;

	CPlayerObject* player = new CPlayerObject(true, Direction);
	player->SetPosition(X, Y);
	player->SetHP(HP);
	player->SetObjectID(ID);
	player->SetDirection(Direction);
	player->ActionInput(dfACTION_STAND);

	if (player->GetDirection()== dfACTION_MOVE_LL)
		player->SetSprite(ePLAYER_STAND_L01, ePLAYER_STAND_L_MAX, dfDELAY_STAND);
	else if (player->GetDirection() == dfACTION_MOVE_RR)
		player->SetSprite(ePLAYER_STAND_R01, ePLAYER_STAND_R_MAX, dfDELAY_STAND);

	g_objectList.push_back(player);
	g_pPlayerObject = player;

	return true;
}

BOOL CNetwork::netPacketProc_CreateOtherCharacter(CPacket * pPacketBuffer)
{
	//stPACKET_SC_CREATE_OTHER_CHARACTER *pPacket = (stPACKET_SC_CREATE_OTHER_CHARACTER *)pPacketBuffer;
	
	DWORD ID;
	BYTE Direction;
	WORD X;
	WORD Y;
	BYTE HP;

	*pPacketBuffer >> ID;
	*pPacketBuffer >> Direction;
	*pPacketBuffer >> X;
	*pPacketBuffer >> Y;
	*pPacketBuffer >> HP;

	CPlayerObject* otherPlayer = new CPlayerObject(true, Direction);
	otherPlayer->SetPosition(X, Y);
	otherPlayer->SetHP(HP);
	otherPlayer->SetObjectID(ID);
	otherPlayer->SetDirection(Direction);
	otherPlayer->ActionInput(dfACTION_STAND);
	
	if (otherPlayer->GetDirection() == dfACTION_MOVE_LL)
		otherPlayer->SetSprite(ePLAYER_STAND_L01, ePLAYER_STAND_L_MAX, dfDELAY_STAND);
	else if (otherPlayer->GetDirection() == dfACTION_MOVE_RR)
		otherPlayer->SetSprite(ePLAYER_STAND_R01, ePLAYER_STAND_R_MAX, dfDELAY_STAND);
	
	g_objectList.push_back(otherPlayer);
	
	return true;
}

BOOL CNetwork::netPacketProc_DeleteCharacter(CPacket * pPacketBuffer)
{
	//stPACKET_SC_DELETE_CHARACTER *pPacket = (stPACKET_SC_DELETE_CHARACTER *)pPacketBuffer;
	DWORD ID;
	BYTE Direction;
	WORD X;
	WORD Y;
	BYTE HP;

	*pPacketBuffer >> ID;
	*pPacketBuffer >> Direction;
	*pPacketBuffer >> X;
	*pPacketBuffer >> Y;
	*pPacketBuffer >> HP;

	list<CBaseObject*>::iterator iter;
	for (iter = g_objectList.begin(); iter != g_objectList.end(); ++iter)
	{
		if ((*iter)->GetObjectID() == ID)
		{
			g_objectList.erase(iter++);
			return true;
		}
	}

	return false;
}

BOOL CNetwork::netPacketProc_MoveCharacter(CPacket * pPacketBuffer)
{
	//stPACKET_SC_MOVE_START *pPacket = (stPACKET_SC_MOVE_START *)pPacketBuffer;
	DWORD ID;
	BYTE Direction;
	WORD X;
	WORD Y;

	*pPacketBuffer >> ID;
	*pPacketBuffer >> Direction;
	*pPacketBuffer >> X;
	*pPacketBuffer >> Y;

	list<CBaseObject*>::iterator iter;
	for (iter = g_objectList.begin(); iter != g_objectList.end(); ++iter)
	{
		if ((*iter)->GetObjectID() == ID)
		{
			(*iter)->ActionInput(Direction);
			(*iter)->SetPosition(X, Y);
			return true;
		}
	}

	return false;
}

BOOL CNetwork::netPacketProc_StopCharacter(CPacket * pPacketBuffer)
{
	//stPACKET_SC_MOVE_STOP *pPacket = (stPACKET_SC_MOVE_STOP *)pPacketBuffer;
	DWORD ID;
	BYTE Direction;
	WORD X;
	WORD Y;

	*pPacketBuffer >> ID;
	*pPacketBuffer >> Direction;
	*pPacketBuffer >> X;
	*pPacketBuffer >> Y;

	list<CBaseObject*>::iterator iter;
	for (iter = g_objectList.begin(); iter != g_objectList.end(); ++iter)
	{
		if ((*iter)->GetObjectID() == ID)
		{
			(*iter)->ActionInput(dfACTION_STAND);
			((CPlayerObject*)(*iter))->SetDirection(Direction);
			(*iter)->SetPosition(X, Y);
			return true;
		}
	}

	return false;
}

BOOL CNetwork::netPacketProc_Attack1(CPacket * pPacketBuffer)
{
	//stPACKET_SC_ATTACK1 *pPacket = (stPACKET_SC_ATTACK1 *)pPacketBuffer;
	DWORD ID;
	BYTE Direction;
	WORD X;
	WORD Y;

	*pPacketBuffer >> ID;
	*pPacketBuffer >> Direction;
	*pPacketBuffer >> X;
	*pPacketBuffer >> Y;

	list<CBaseObject*>::iterator iter;
	for (iter = g_objectList.begin(); iter != g_objectList.end(); ++iter)
	{
		if ((*iter)->GetObjectID() == ID)
		{
			(*iter)->ActionInput(dfACTION_ATTACK1);
			((CPlayerObject*)(*iter))->SetDirection(Direction);
			(*iter)->SetPosition(X, Y);
			return true;
		}
	}

	return false;
}

BOOL CNetwork::netPacketProc_Attack2(CPacket * pPacketBuffer)
{
	//stPACKET_SC_ATTACK2 *pPacket = (stPACKET_SC_ATTACK2 *)pPacketBuffer;
	DWORD ID;
	BYTE Direction;
	WORD X;
	WORD Y;

	*pPacketBuffer >> ID;
	*pPacketBuffer >> Direction;
	*pPacketBuffer >> X;
	*pPacketBuffer >> Y;

	list<CBaseObject*>::iterator iter;
	for (iter = g_objectList.begin(); iter != g_objectList.end(); ++iter)
	{
		if ((*iter)->GetObjectID() == ID)
		{
			(*iter)->ActionInput(dfACTION_ATTACK2);
			((CPlayerObject*)(*iter))->SetDirection(Direction);
			(*iter)->SetPosition(X,Y);
			return true;
		}
	}

	return false;
}

BOOL CNetwork::netPacketProc_Attack3(CPacket * pPacketBuffer)
{
	//stPACKET_SC_ATTACK3 *pPacket = (stPACKET_SC_ATTACK3 *)pPacketBuffer;
	DWORD ID;
	BYTE Direction;
	WORD X;
	WORD Y;

	*pPacketBuffer >> ID;
	*pPacketBuffer >> Direction;
	*pPacketBuffer >> X;
	*pPacketBuffer >> Y;

	list<CBaseObject*>::iterator iter;
	for (iter = g_objectList.begin(); iter != g_objectList.end(); ++iter)
	{
		if ((*iter)->GetObjectID() == ID)
		{
			(*iter)->ActionInput(dfACTION_ATTACK3);
			((CPlayerObject*)(*iter))->SetDirection(Direction);
			(*iter)->SetPosition(X, Y);
			return true;
		}
	}

	return false;
}

BOOL CNetwork::netPacketProc_Damage(CPacket * pPacketBuffer)
{
	//stPACKET_SC_DAMAGE *pPacket = (stPACKET_SC_DAMAGE *)pPacketBuffer;
	DWORD AttackID;
	DWORD DamageID;
	BYTE DamageHP;

	*pPacketBuffer >> AttackID;
	*pPacketBuffer >> DamageID;
	*pPacketBuffer >> DamageHP;



	list<CBaseObject*>::iterator iter;
	for (iter = g_objectList.begin(); iter != g_objectList.end(); ++iter)
	{
		if ((*iter)->GetObjectID() == DamageID)
		{
			//����Ʈ ����
			CPlayerObject* damagedPlayer = (CPlayerObject*)(*iter);
			CBaseObject *pEffect = new CEffectObject(-1, damagedPlayer->GetCurX(), damagedPlayer->GetCurY() - 50, dfDELAY_ATTACK3, eEFFECT_SPARK_01, eEFFECT_SPARK_04);
			((CPlayerObject*)(*iter))->SetHP(DamageHP);

			g_objectList.push_back(pEffect);
			return true;
		}
	}

	return false;
}

BOOL CNetwork::SendPacket(stNETWORK_PACKET_HEADER * pHeader, CPacket * pPacket)
{
	//���ӻ��� ����ó��

	int ret;
	//sendQ�� ��� �ֱ�
	ret = g_SendBuffer.Enqueue((char*)pHeader, sizeof(stNETWORK_PACKET_HEADER));

	//sendQ�� ��Ŷ �ֱ�
	ret = g_SendBuffer.Enqueue((char*)pPacket->GetBufferPtr(), pHeader->bySize);

	//sendQ�� EndCode �ֱ�
	BYTE endCode = dfNETWORK_PACKET_END;
	ret = g_SendBuffer.Enqueue((char*)&endCode, sizeof(dfNETWORK_PACKET_END));

	//3ȸ�� ���ļ� ���, ������, �����ڵ带 SendQ�� �ִٺ��� ��ȿ�������� ���� �� ����
	//���� ���ظ� ���� �̷��� ���� 
	//���� ����ȭ ����

	
	//send�� �õ��ϴ� �Լ� ȣ��
	SendEvent();


	return 0;
}

void CNetwork::mpMoveStart(stNETWORK_PACKET_HEADER * pHeader, CPacket * pPacket, int _dir, short _x, short _y)
{
	*pPacket << (BYTE)_dir;
	*pPacket << (WORD)_x;
	*pPacket << (WORD)_y;

	pHeader->byCode = dfNETWORK_PACKET_CODE;
	pHeader->bySize = pPacket->GetDataSize();
	pHeader->byType = dfPACKET_CS_MOVE_START;
}

void CNetwork::mpMoveStop(stNETWORK_PACKET_HEADER * pHeader, CPacket * pPacket, int _dir, short _x, short _y)
{
	*pPacket << (BYTE)_dir;
	*pPacket << (WORD)_x;
	*pPacket << (WORD)_y;

	pHeader->byCode = dfNETWORK_PACKET_CODE;
	pHeader->bySize = pPacket->GetDataSize();
	pHeader->byType = dfPACKET_CS_MOVE_STOP;
}

void CNetwork::mpAttack1(stNETWORK_PACKET_HEADER *pHeader, CPacket *pPacket, int _dir, short _x, short _y)
{
	*pPacket << (BYTE)_dir;
	*pPacket << (WORD)_x;
	*pPacket << (WORD)_y;

	pHeader->byCode = dfNETWORK_PACKET_CODE;
	pHeader->bySize = pPacket->GetDataSize();
	pHeader->byType = dfPACKET_CS_ATTACK1;
}

void CNetwork::mpAttack2(stNETWORK_PACKET_HEADER *pHeader, CPacket *pPacket, int _dir, short _x, short _y)
{
	*pPacket << (BYTE)_dir;
	*pPacket << (WORD)_x;
	*pPacket << (WORD)_y;

	pHeader->byCode = dfNETWORK_PACKET_CODE;
	pHeader->bySize = pPacket->GetDataSize();
	pHeader->byType = dfPACKET_CS_ATTACK2;

}

void CNetwork::mpAttack3(stNETWORK_PACKET_HEADER *pHeader, CPacket *pPacket, int _dir, short _x, short _y)
{
	*pPacket << (BYTE)_dir;
	*pPacket << (WORD)_x;
	*pPacket << (WORD)_y;
	
	pHeader->byCode = dfNETWORK_PACKET_CODE;
	pHeader->bySize = pPacket->GetDataSize();
	pHeader->byType = dfPACKET_CS_ATTACK3;
}
