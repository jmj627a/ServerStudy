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

		//recv한게 다 enq 될 때까지. 이건 바로 링버퍼에 recv하면 사라질 문제
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

			//완료 패킷 처리부
			while (true)
			{
				//1. RecvQ에 최소한의 사이즈가 있는지 확인. 헤더의 길이보다 작으면 아직 덜왔다 판단 후 나가기
				if (g_RecvBuffer.GetUseSize() <= sizeof(st_PACKET_HEADER)) break;

				//printf("recv size ok \n");

				//2. RecvQ에서 헤더를 Peek
				ret = g_RecvBuffer.Peek((char*)&Header, sizeof(st_PACKET_HEADER));

				//unsigned char* ptr = (unsigned char*)&Header;
				//printf("header");
				//for (int i = 0; i < sizeof(st_PACKET_HEADER); i++)
				//{
				//	printf(" : %02X", ptr[i]);
				//}
				//printf("\n");

				//3. 헤더의 code 확인
				if (Header.byCode != dfPACKET_CODE) 
				{
					g_RecvBuffer.MoveFront(1);
					continue;
					//return false;
				}

				//printf("recv header ok \n");

				//4. 헤더의 len 값과 RecvQ의 데이터 사이즈 비교 (헤더 + len + 엔드코드)  패킷 하나가 완성이 안됐으면(헤더에 담긴 패킷 길이 + 헤더 크기) 할 게 없으니 나가기
				if (g_RecvBuffer.GetUseSize() < sizeof(Header) + Header.bySize + sizeof(BYTE)) return false;

				//printf("recv len ok \n");

				//헤더 지나서 패킷만 읽음
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
				catch (...)//오류받는 구조체 만들어야함
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

//FD_WRITE가 떴을 때. 이 함수는 sendQueue.GetUseSize() <=0 될 때 까지 while문 돌려야 한다. 아니면 우드블럭 뜰 때 까지.
bool CNetwork::SendEvent()
{
	try
	{
		if (!g_SendPossible)
		{
			printf("3 g_SendPossible false \n");
			return false;
		}
		//큐에 있는 것 보내주기 
		while (true)
		{
			char sendBuffer[1000]; //이걸 거쳐서 send 하는데 여기선 peek로 확인 
			int ret = g_SendBuffer.Peek(sendBuffer, 1000);

			ret = send(Socket, sendBuffer, ret, 0);

			if (ret == SOCKET_ERROR)
			{
				int err = WSAGetLastError();

				//우드블럭이 아니라면 
				if (err != WSAEWOULDBLOCK)
				{
					printf("4 NOT WOULD BLOCK %d\n", err);
					return false;
				}
				//우드블럭이 맞다면 -> 아무것도 카피를 못한 것. FD_WRITE가 뜨기 전까지 우드블럭, 더 확인할 필요 없다. isSend=false로 바꾸고 그냥 나간다. 나중에 FD_WRITE에서 true로 바꿔줄거다. 
				else
				{
					printf("5 WOULD BLOCK \n");
					g_SendPossible = false;
					return false;
				}

			}

			//에러가 나지 않았으면 뭐라도 갔다는 것. sendQueue에서 removeFront(ret)로 copy에 성공한 ret만큼만 지워줌
			g_SendBuffer.MoveFront(ret);


			printf("2 send buffer size : %d \n", g_SendBuffer.GetUseSize());

			//session->SendQ 를 모두 보내면 반복문 중단
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

		//클라에서 처리해야할 패킷별로 처리 함수 호출
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
			//이펙트 생성
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
	//접속상태 예외처리

	printf("[send : %d]\n", count++);

	int ret;
	//sendQ에 헤더 넣기
	ret = g_SendBuffer.Enqueue((char*)pHeader, sizeof(st_PACKET_HEADER));
	if (ret != sizeof(st_PACKET_HEADER))
	{
		printf("fail to send header\n");
		g_SendBuffer.ClearBuffer();
		return false;
	}

	//sendQ에 패킷 넣기
	ret = g_SendBuffer.Enqueue((char*)pPacket->GetBufferPtr(), pHeader->bySize);

	//sendQ에 EndCode 넣기
	BYTE endCode = dfNETWORK_PACKET_END;
	ret = g_SendBuffer.Enqueue((char*)&endCode, sizeof(dfNETWORK_PACKET_END));

	//3회에 걸쳐서 헤더, 데이터, 엔드코드를 SendQ에 넣다보니 비효율적으로 보일 수 있음
	//빠른 이해를 위해 이렇게 구현 
	//차후 직렬화 버퍼

	//printf("1 send buffer size : %d \n", g_SendBuffer.GetUseSize());

	//char tempbuf[1000] = { 0, };
	//g_SendBuffer.Peek(tempbuf, g_SendBuffer.GetUseSize());
	//printf("buffer");
	//for (int i = 0; i < g_SendBuffer.GetUseSize(); i++)
	//{
	//	printf(" : %02X", (unsigned char)tempbuf[i]);
	//}
	//printf("\n");

	//send를 시도하는 함수 호출
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
