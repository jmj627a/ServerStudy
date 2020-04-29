#pragma once
#include "stdafx.h"
#include "CNetwork.h"

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

	ret = g_RecvBuffer.Enqueue(recvBuffer, ret);

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

		char *Packet = (char*)malloc(Header.bySize);
		ret = g_RecvBuffer.Dequeue(Packet, Header.bySize);

		BYTE endCode;
		g_RecvBuffer.Peek((char*)&endCode, 1);
		if (endCode != dfNETWORK_PACKET_END) return false;
		g_RecvBuffer.MoveFront(sizeof(endCode));

		PacketProc(Header.byType, Packet);
		
		delete Packet;
	}

}

//FD_WRITE�� ���� ��. �� �Լ��� sendQueue.GetUseSize() <=0 �� �� ���� while�� ������ �Ѵ�. �ƴϸ� ������ �� �� ����.
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

			//�������� �ƴ϶�� 
			if (err != WSAEWOULDBLOCK)
				return false;
			//�������� �´ٸ� -> �ƹ��͵� ī�Ǹ� ���� ��. FD_WRITE�� �߱� ������ ������, �� Ȯ���� �ʿ� ����. isSend=false�� �ٲٰ� �׳� ������. ���߿� FD_WRITE���� true�� �ٲ��ٰŴ�. 
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


void CNetwork::PacketProc(BYTE byPacketType, char* Packet)
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
		break;

	case dfPACKET_SC_ATTACK2:
		break;

	case dfPACKET_SC_ATTACK3:
		break;

	case dfPACKET_SC_DAMAGE:
		break;

		//Ŭ�󿡼� ó���ؾ��� ��Ŷ���� ó�� �Լ� ȣ��
	}

}

BOOL CNetwork::netPacketProc_CreateMyCharacter(char *pPacketBuffer)
{
	stPACKET_SC_CREATE_MY_CHARACTER * pPacket = (stPACKET_SC_CREATE_MY_CHARACTER*)pPacketBuffer;

	CPlayerObject* player = new CPlayerObject();
	player->SetPosition(pPacket->X, pPacket->Y);
	player->SetHP(pPacket->HP);
	player->SetObjectID(pPacket->ID);
	player->SetDirection(pPacket->Direction);
	player->ActionInput(dfACTION_STAND);
	player->SetSprite(ePLAYER_STAND_R01, ePLAYER_STAND_R_MAX, dfDELAY_STAND);

	g_objectList.push_back(player);
	g_pPlayerObject = player;

	return true;
}

BOOL CNetwork::netPacketProc_CreateOtherCharacter(char * pPacketBuffer)
{
	
	stPACKET_SC_CREATE_OTHER_CHARACTER *pPacket = (stPACKET_SC_CREATE_OTHER_CHARACTER *)pPacketBuffer;
	
	CPlayerObject* otherPlayer = new CPlayerObject();
	otherPlayer->SetPosition(pPacket->X, pPacket->Y);
	otherPlayer->SetHP(pPacket->HP);
	otherPlayer->SetObjectID(pPacket->ID);
	otherPlayer->SetDirection(pPacket->Direction);
	otherPlayer->ActionInput(dfACTION_STAND);
	otherPlayer->SetSprite(ePLAYER_STAND_R01, ePLAYER_STAND_R_MAX, dfDELAY_STAND);
	
	g_objectList.push_back(otherPlayer);
	
	return true;
}

BOOL CNetwork::netPacketProc_DeleteCharacter(char * pPacketBuffer)
{
	stPACKET_SC_DELETE_CHARACTER *pPacket = (stPACKET_SC_DELETE_CHARACTER *)pPacketBuffer;

	list<CBaseObject*>::iterator iter;
	for (iter = g_objectList.begin(); iter != g_objectList.end(); ++iter)
	{
		if ((*iter)->GetObjectID() == pPacket->ID)
		{
			g_objectList.remove(*iter);
			return true;
		}
	}

	return false;
}

BOOL CNetwork::netPacketProc_MoveCharacter(char * pPacketBuffer)
{
	stPACKET_SC_MOVE_START *pPacket = (stPACKET_SC_MOVE_START *)pPacketBuffer;

	list<CBaseObject*>::iterator iter;
	for (iter = g_objectList.begin(); iter != g_objectList.end(); ++iter)
	{
		if ((*iter)->GetObjectID() == pPacket->ID)
		{
			(*iter)->ActionInput(pPacket->Direction);
			(*iter)->SetPosition(pPacket->X, pPacket->Y);
			return true;
		}
	}

	return false;
}

BOOL CNetwork::netPacketProc_StopCharacter(char * pPacketBuffer)
{
	stPACKET_SC_MOVE_STOP *pPacket = (stPACKET_SC_MOVE_STOP *)pPacketBuffer;

	list<CBaseObject*>::iterator iter;
	for (iter = g_objectList.begin(); iter != g_objectList.end(); ++iter)
	{
		if ((*iter)->GetObjectID() == pPacket->ID)
		{
			(*iter)->ActionInput(dfACTION_STAND);
			((CPlayerObject*)(*iter))->SetDirection(pPacket->Direction);
			(*iter)->SetPosition(pPacket->X, pPacket->Y);
			return true;
		}
	}

	return false;
}

BOOL CNetwork::SendPacket(stNETWORK_PACKET_HEADER * pHeader, char * pPacket)
{
	//���ӻ��� ����ó��

	int ret;
	//sendQ�� ��� �ֱ�
	ret = g_SendBuffer.Enqueue((char*)pHeader, sizeof(stNETWORK_PACKET_HEADER));

	//sendQ�� ��Ŷ �ֱ�
	ret = g_SendBuffer.Enqueue((char*)pPacket, pHeader->bySize);

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

void CNetwork::mpMoveStart(stNETWORK_PACKET_HEADER * pHeader, stPACKET_CS_MOVE_START * pPacket, int _dir, short _x, short _y)
{
	pHeader->byCode = dfNETWORK_PACKET_CODE;
	pHeader->bySize = sizeof(stPACKET_CS_MOVE_START);
	pHeader->byType = dfPACKET_CS_MOVE_START;

	pPacket->Direction = _dir;
	pPacket->X = _x;
	pPacket->Y = _y;
}

void CNetwork::mpMoveStop(stNETWORK_PACKET_HEADER * pHeader, stPACKET_CS_MOVE_STOP * pPacket, bool _dir, short _x, short _y)
{
	pHeader->byCode = dfNETWORK_PACKET_CODE;
	pHeader->bySize = sizeof(stPACKET_CS_MOVE_STOP);
	pHeader->byType = dfPACKET_CS_MOVE_STOP;

	pPacket->Direction = _dir;
	pPacket->X = _x;
	pPacket->Y = _y;
}