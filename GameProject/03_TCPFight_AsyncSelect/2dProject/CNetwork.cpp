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

	//완료 패킷 처리부
	while (true)
	{
		//1. RecvQ에 최소한의 사이즈가 있는지 확인. 헤더의 길이보다 작으면 아직 덜왔다 판단 후 나가기
		if (g_RecvBuffer.GetUseSize() <= 2) break;

		//2. RecvQ에서 헤더를 Peek
		g_RecvBuffer.Peek((char*)&Header, 2);

		//3. 헤더의 code 확인
		if (Header.byCode != '0x89') return false;

		//4. 헤더의 len 값과 RecvQ의 데이터 사이즈 비교 (헤더 + len + 엔드코드)  패킷 하나가 완성이 안됐으면(헤더에 담긴 패킷 길이 + 헤더 크기) 할 게 없으니 나가기
		if (g_RecvBuffer.GetUseSize() < sizeof(Header) + Header.bySize + sizeof(BYTE)) return;

		//헤더 지나서 패킷만 읽음
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

//FD_WRITE가 떴을 때. 이 함수는 sendQueue.GetUseSize() <=0 될 때 까지 while문 돌려야 한다. 아니면 우드블럭 뜰 때 까지.
bool CNetwork::SendEvent()
{
	if (!g_SendPossible)
		return false;

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
				return;
			//우드블럭이 맞다면 -> 아무것도 카피를 못한 것. FD_WRITE가 뜨기 전까지 우드블럭, 더 확인할 필요 없다. isSend=false로 바꾸고 그냥 나간다. 나중에 FD_WRITE에서 true로 바꿔줄거다. 
			else
			{
				g_SendPossible = false;
				return false;
			}

		}

		//에러가 나지 않았으면 뭐라도 갔다는 것. sendQueue에서 removeFront(ret)로 copy에 성공한 ret만큼만 지워줌
		g_SendBuffer.MoveFront(ret);

		//session->SendQ 를 모두 보내면 반복문 중단
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

		//클라에서 처리해야할 패킷별로 처리 함수 호출
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
	//접속상태 예외처리

	//sendQ에 헤더 넣기
	g_SendBuffer.Enqueue((char*)pHeader, sizeof(pHeader));

	//sendQ에 패킷 넣기
	g_SendBuffer.Enqueue((char*)pPacket, sizeof(pPacket));

	//sendQ에 EndCode 넣기
	char endCode = dfNETWORK_PACKET_END;
	g_SendBuffer.Enqueue(&endCode, sizeof(endCode));

	//3회에 걸쳐서 헤더, 데이터, 엔드코드를 SendQ에 넣다보니 비효율적으로 보일 수 있음
	//빠른 이해를 위해 이렇게 구현 
	//차후 직렬화 버퍼

	
	//send를 시도하는 함수 호출
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
