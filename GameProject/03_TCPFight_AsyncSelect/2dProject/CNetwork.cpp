#pragma once
#include "framework.h"
#include "CNetwork.h"

extern CRingBuffer g_RecvBuffer;
extern CRingBuffer g_SendBuffer;
extern bool g_SendPossible = false;
extern SOCKET Socket;

extern list<CPlayerObject*> g_objectList;

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
		if (g_RecvBuffer.GetUseSize() <= 2) break;

		//2. RecvQ���� ����� Peek
		g_RecvBuffer.Peek((char*)&Header, 2);

		//3. ����� code Ȯ��
		if (Header.byCode != '0x89') return false;

		//4. ����� len ���� RecvQ�� ������ ������ �� (��� + len + �����ڵ�)  ��Ŷ �ϳ��� �ϼ��� �ȵ�����(����� ��� ��Ŷ ���� + ��� ũ��) �� �� ������ ������
		if (g_RecvBuffer.GetUseSize() < sizeof(Header) + Header.bySize + sizeof(BYTE)) return;

		//��� ������ ��Ŷ�� ����
		g_RecvBuffer.MoveFront(sizeof(Header));

		char *Packet = (char*)malloc(Header.bySize);
		g_RecvBuffer.Dequeue((char*)&Packet, Header.bySize);

		BYTE endCode;
		g_RecvBuffer.Peek((char*)&endCode, 1);
		if (endCode != '0x79') return false;
		g_RecvBuffer.MoveFront(sizeof(endCode));

		PacketProc(Header.byType, Packet);
		
		delete Packet;
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
				return;
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
		break;

	case dfPACKET_SC_MOVE_STOP:
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

BOOL CNetwork::netPacketProc_CreateMyCharacter(char * pPacketBuffer)
{
	stPACKET_SC_CREATE_MY_CHARACTER *pPacket = (stPACKET_SC_CREATE_MY_CHARACTER *)pPacketBuffer;

	CPlayerObject* player = new CPlayerObject();
	player->SetPosition(pPacket->X, pPacket->Y);
	player->SetHP(pPacket->HP);
	player->SetObjectID(pPacket->ID);
	player->SetDirection(pPacket->Direction);
	player->ActionInput(dfACTION_STAND);
	player->SetSprite(ePLAYER_STAND_R01, ePLAYER_STAND_R_MAX, dfDELAY_STAND);

	g_objectList.push_back(player);

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
	
	return 0;
}

BOOL CNetwork::netPacketProc_DeleteCharacter(char * pPacketBuffer)
{
	return 0;
}

BOOL CNetwork::SendPacket(stNETWORK_PACKET_HEADER * pHeader, char * pPacket)
{
	//���ӻ��� ����ó��

	//sendQ�� ��� �ֱ�
	g_SendBuffer.Enqueue((char*)pHeader, sizeof(pHeader));

	//sendQ�� ��Ŷ �ֱ�
	g_SendBuffer.Enqueue((char*)pPacket, sizeof(pPacket));

	//sendQ�� EndCode �ֱ�
	char endCode = dfNETWORK_PACKET_END;
	g_SendBuffer.Enqueue(&endCode, sizeof(endCode));

	//3ȸ�� ���ļ� ���, ������, �����ڵ带 SendQ�� �ִٺ��� ��ȿ�������� ���� �� ����
	//���� ���ظ� ���� �̷��� ���� 
	//���� ����ȭ ����

	
	//send�� �õ��ϴ� �Լ� ȣ��
	SendEvent();


	return 0;
}

void CNetwork::mpMoveStart(stNETWORK_PACKET_HEADER * pHeader, stPACKET_CS_MOVE_START * pPacket, bool _dir, short _x, short _y)
{
	pHeader->byCode = dfNETWORK_PACKET_CODE;
	pHeader->bySize = 5;
	pHeader->byTemp = dfPACKET_CS_MOVE_START;

	pPacket->Direction = _dir;
	pPacket->X = _x;
	pPacket->Y = _y;
}
