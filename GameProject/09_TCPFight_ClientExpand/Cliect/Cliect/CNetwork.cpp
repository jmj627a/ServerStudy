#pragma once
#include "stdafx.h"
#include "CNetwork.h"
#include "CEffectObject.h"
#include "CMap.h"

extern RingBuf g_RecvBuffer;
extern RingBuf g_SendBuffer;

extern bool g_SendPossible;
extern SOCKET Socket;

extern list<CBaseObject*> g_objectList;
extern CPlayerObject* g_pPlayerObject;
extern CMap map;

CNetwork::CNetwork()
{
}


CNetwork::~CNetwork()
{
}

bool CNetwork::RecvEvent()
{
	static int count = 0;
	try
	{
		printf("[recv : %d]\n", count++);

		char recvBuffer[1000];
		int ret = recv(Socket, recvBuffer, 1000, 0);
		if (ret == SOCKET_ERROR)
		{
			printf("recv socket error \n");
			return false;
		}
		char* pbuf = recvBuffer;

		//recv�Ѱ� �� enq �� ������. �̰� �ٷ� �����ۿ� recv�ϸ� ����� ����
		//while (ret != 0)
		{
			ret = g_RecvBuffer.Enqueue(pbuf, ret);
			printf("recv enqueue size %d \n", ret);

			//unsigned char* ptr = (unsigned char*)g_RecvBuffer.arr;
			//printf("RAW");
			//for (int i = 0; i < BUFFER_SIZE; i++)
			//{
			//	printf(" : %02X", ptr[i]);
			//}
			//printf("\n");
			//printf("R-POS : %d \n", g_RecvBuffer.readPos);
			//printf("W-POS : %d \n", g_RecvBuffer.writePos);


			st_PACKET_HEADER Header;

			//�Ϸ� ��Ŷ ó����
			while (true)
			{
				//1. RecvQ�� �ּ����� ����� �ִ��� Ȯ��. ����� ���̺��� ������ ���� ���Դ� �Ǵ� �� ������
				if (g_RecvBuffer.GetUseSize() <= sizeof(st_PACKET_HEADER)) break;

				//printf("recv size ok \n");

				//2. RecvQ���� ����� Peek
				ret = g_RecvBuffer.Peek((char*)&Header, sizeof(st_PACKET_HEADER));

				//unsigned char* ptr = (unsigned char*)&Header;
				//printf("header");
				//for (int i = 0; i < sizeof(st_PACKET_HEADER); i++)
				//{
				//	printf(" : %02X", ptr[i]);
				//}
				//printf("\n");

				//3. ����� code Ȯ��
				if (Header.byCode != dfPACKET_CODE) 
				{
					g_RecvBuffer.MoveFront(1);
					continue;
					//return false;
				}

				//printf("recv header ok \n");

				//4. ����� len ���� RecvQ�� ������ ������ �� (��� + len + �����ڵ�)  ��Ŷ �ϳ��� �ϼ��� �ȵ�����(����� ��� ��Ŷ ���� + ��� ũ��) �� �� ������ ������
				if (g_RecvBuffer.GetUseSize() < sizeof(Header) + Header.bySize + sizeof(BYTE)) return false;

				//printf("recv len ok \n");

				//��� ������ ��Ŷ�� ����
				g_RecvBuffer.MoveFront(sizeof(Header));

				//char* Packet = (char*)malloc(Header.bySize);
				CPacket Packet;

				ret = g_RecvBuffer.Dequeue((char*)Packet.GetBufferPtr(), Header.bySize);

				//printf("recv dequeue size %d \n", ret);

				BYTE endCode;
				g_RecvBuffer.Peek((char*)&endCode, 1);
				g_RecvBuffer.MoveFront(sizeof(endCode));

				if (endCode != dfNETWORK_PACKET_END) return false;

				//printf("recv packet end ok \n");


				try {
					PacketProc(Header.byType, &Packet);
				}
				catch (...)//�����޴� ����ü ��������
				{
					printf("packet proc expection \n");
				}
			}
			return true;
		}
	}
	catch(...)
	{
		printf("recv expection \n");
	}

	return false;
}

//FD_WRITE�� ���� ��. �� �Լ��� sendQueue.GetUseSize() <=0 �� �� ���� while�� ������ �Ѵ�. �ƴϸ� ���� �� �� ����.
bool CNetwork::SendEvent()
{
	try
	{
		if (!g_SendPossible)
		{
			printf("3 g_SendPossible false \n");
			return false;
		}
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
				{
					printf("4 NOT WOULD BLOCK %d\n", err);
					return false;
				}
				//������ �´ٸ� -> �ƹ��͵� ī�Ǹ� ���� ��. FD_WRITE�� �߱� ������ ����, �� Ȯ���� �ʿ� ����. isSend=false�� �ٲٰ� �׳� ������. ���߿� FD_WRITE���� true�� �ٲ��ٰŴ�. 
				else
				{
					printf("5 WOULD BLOCK \n");
					g_SendPossible = false;
					return false;
				}

			}

			//������ ���� �ʾ����� ���� ���ٴ� ��. sendQueue���� removeFront(ret)�� copy�� ������ ret��ŭ�� ������
			g_SendBuffer.MoveFront(ret);


			printf("2 send buffer size : %d \n", g_SendBuffer.GetUseSize());

			//session->SendQ �� ��� ������ �ݺ��� �ߴ�
			if (g_SendBuffer.GetUseSize() == 0)
				return true;
		}
	}
	catch (...)
	{
		printf("send event catch \n");
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
			(*iter)->SetPosition(X, Y);
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

			int startX = (damagedPlayer->GetCurX() - map.m_DrawX);
			int startY = (damagedPlayer->GetCurY() - map.m_DrawY);

			if (map.m_DrawX == dfRANGE_MOVE_RIGHT - dfSCREEN_WIDTH / 2)
				startX += dfSCREEN_WIDTH / 2;
			if (map.m_DrawY == dfRANGE_MOVE_BOTTOM - dfSCREEN_HEIGHT / 2)
				startY += dfSCREEN_HEIGHT / 2;

			if (startX < 0 || startX > dfSCREEN_WIDTH)
				return false;

			if (startY < 0 || startY > dfSCREEN_HEIGHT)
				return false;

			CBaseObject *pEffect = new CEffectObject(-1, startX, startY - 50, dfDELAY_ATTACK3, eEFFECT_SPARK_01, eEFFECT_SPARK_04);
			((CPlayerObject*)(*iter))->SetHP(DamageHP);

			g_objectList.push_back(pEffect);
			return true;
		}
	}

	return false;
}

BOOL CNetwork::SendPacket(st_PACKET_HEADER * pHeader, CPacket * pPacket)
{
	static int count = 0;
	//���ӻ��� ����ó��

	printf("[send : %d]\n", count++);

	int ret;
	//sendQ�� ��� �ֱ�
	ret = g_SendBuffer.Enqueue((char*)pHeader, sizeof(st_PACKET_HEADER));
	if (ret != sizeof(st_PACKET_HEADER))
	{
		printf("fail to send header\n");
		g_SendBuffer.ClearBuffer();
		return false;
	}

	//sendQ�� ��Ŷ �ֱ�
	ret = g_SendBuffer.Enqueue((char*)pPacket->GetBufferPtr(), pHeader->bySize);

	//sendQ�� EndCode �ֱ�
	BYTE endCode = dfNETWORK_PACKET_END;
	ret = g_SendBuffer.Enqueue((char*)&endCode, sizeof(dfNETWORK_PACKET_END));

	//3ȸ�� ���ļ� ���, ������, �����ڵ带 SendQ�� �ִٺ��� ��ȿ�������� ���� �� ����
	//���� ���ظ� ���� �̷��� ���� 
	//���� ����ȭ ����

	//printf("1 send buffer size : %d \n", g_SendBuffer.GetUseSize());

	//char tempbuf[1000] = { 0, };
	//g_SendBuffer.Peek(tempbuf, g_SendBuffer.GetUseSize());
	//printf("buffer");
	//for (int i = 0; i < g_SendBuffer.GetUseSize(); i++)
	//{
	//	printf(" : %02X", (unsigned char)tempbuf[i]);
	//}
	//printf("\n");

	//send�� �õ��ϴ� �Լ� ȣ��
	SendEvent();


	return 0;
}

void CNetwork::mpMoveStart(st_PACKET_HEADER * pHeader, CPacket * pPacket, int _dir, short _x, short _y)
{
	*pPacket << (BYTE)_dir;
	*pPacket << (WORD)_x;
	*pPacket << (WORD)_y;

	pHeader->byCode = dfPACKET_CODE;
	pHeader->bySize = pPacket->GetDataSize();
	pHeader->byType = dfPACKET_CS_MOVE_START;
}

void CNetwork::mpMoveStop(st_PACKET_HEADER * pHeader, CPacket * pPacket, int _dir, short _x, short _y)
{
	*pPacket << (BYTE)_dir;
	*pPacket << (WORD)_x;
	*pPacket << (WORD)_y;

	pHeader->byCode = dfPACKET_CODE;
	pHeader->bySize = pPacket->GetDataSize();
	pHeader->byType = dfPACKET_CS_MOVE_STOP;
}

void CNetwork::mpAttack1(st_PACKET_HEADER *pHeader, CPacket *pPacket, int _dir, short _x, short _y)
{
	*pPacket << (BYTE)_dir;
	*pPacket << (WORD)_x;
	*pPacket << (WORD)_y;

	pHeader->byCode = dfPACKET_CODE;
	pHeader->bySize = pPacket->GetDataSize();
	pHeader->byType = dfPACKET_CS_ATTACK1;
}

void CNetwork::mpAttack2(st_PACKET_HEADER *pHeader, CPacket *pPacket, int _dir, short _x, short _y)
{
	*pPacket << (BYTE)_dir;
	*pPacket << (WORD)_x;
	*pPacket << (WORD)_y;

	pHeader->byCode = dfPACKET_CODE;
	pHeader->bySize = pPacket->GetDataSize();
	pHeader->byType = dfPACKET_CS_ATTACK2;

}

void CNetwork::mpAttack3(st_PACKET_HEADER *pHeader, CPacket *pPacket, int _dir, short _x, short _y)
{
	*pPacket << (BYTE)_dir;
	*pPacket << (WORD)_x;
	*pPacket << (WORD)_y;
	
	pHeader->byCode = dfPACKET_CODE;
	pHeader->bySize = pPacket->GetDataSize();
	pHeader->byType = dfPACKET_CS_ATTACK3;
}
