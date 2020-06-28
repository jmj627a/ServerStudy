#include "Network.h"

//출력 저장 대상의 로그 레벨
extern int g_iLogLevel;

//로그 저장시 필요한 임시 버퍼
extern WCHAR g_szLogBuff[1024];

SOCKET g_ListenSocket = INVALID_SOCKET;
std::unordered_map<SOCKET, st_SESSION *> g_SessionMap;

extern std::unordered_map<DWORD, st_CHARACTER*> g_CharacterMap;
extern std::list<st_CHARACTER*> g_Sector[dfSECTOR_MAX_Y][dfSECTOR_MAX_X];

int g_SessionCount = 0;

int netStartUp()
{
	WSADATA wsaData;
	SOCKADDR_IN servAddr;

	int ret;
	// 소켓 라이브러리 초기화
	ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret != NO_ERROR) {
		wprintf(L"WSAStartup failed with error: %ld\n", ret);
		return -1;
	}

	// 소켓 생성
	g_ListenSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (g_ListenSocket == INVALID_SOCKET)
		return -1;

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(dfNETWORK_PORT);

	// IP 주소 PORT번호 할당
	ret = bind(g_ListenSocket, (SOCKADDR*)&servAddr, sizeof(servAddr));
	if (ret == SOCKET_ERROR)
		return -1;

	// 생성한 소켓을 서버 소켓으로 완성
	ret = listen(g_ListenSocket, SOMAXCONN);
	if (ret == SOCKET_ERROR)
		return -1;


	u_long on = 1;
	ioctlsocket(g_ListenSocket, FIONBIO, &on);

	return 1;
}

void netCleanUp()
{
	closesocket(g_ListenSocket);
	WSACleanup();
}

st_SESSION * FindSession(SOCKET socket)
{
	auto Finditer = g_SessionMap.find(socket);

	if (Finditer != g_SessionMap.end())
		return Finditer->second;

	return nullptr;
}

void netIOProcess(void)
{
	st_SESSION *pSession;

	SOCKET UserTable_SOCKET[FD_SETSIZE] = { INVALID_SOCKET };
	int iSocketCount = 0;

	//FD_SET은 FD_SIZE만큼씩만 소켓 검사가 가능하다
	//그러므로 그 개수만큼 넣어서 사용함
	FD_SET ReadSet;
	FD_SET WriteSet;

	FD_ZERO(&ReadSet);
	FD_ZERO(&WriteSet);

	//리슨 소켓 넣기
	FD_SET(g_ListenSocket, &ReadSet);
	UserTable_SOCKET[iSocketCount] = g_ListenSocket;
	iSocketCount++;

	//리슨 소켓 및 접속중인 모든 클라이언트에 대해 SOCKET을 체크한다
	std::unordered_map<SOCKET, st_SESSION*>::iterator sessionIter;
	for (sessionIter = g_SessionMap.begin(); sessionIter != g_SessionMap.end(); )
	{
		pSession = sessionIter->second;

		//해당 클라이언트 ReadSet 등록 / SendQ에 데이터가 있다면 WriteSet 등록
		UserTable_SOCKET[iSocketCount] = pSession->socket;

		FD_SET(pSession->socket, &ReadSet);
		if(pSession->SendQ.GetUseSize() > 0)
			FD_SET(pSession->socket, &WriteSet);

		iSocketCount++;
		sessionIter++;			//미리 증가시켜둬서 밑에서 삭제되어도 iter는 지장없도록

		//select 최대치 도달. 만들어진 테이블 정보로 select 함수 호출 후 정리
		if (FD_SETSIZE <= iSocketCount)
		{
			netSelectSocket(UserTable_SOCKET, &ReadSet, &WriteSet);

			FD_ZERO(&ReadSet);
			FD_ZERO(&WriteSet);

			memset(UserTable_SOCKET, INVALID_SOCKET, sizeof(SOCKET) * FD_SETSIZE);

			//64개마다 accept처리를 해주기 위해 listen Socket을 넣음
			FD_SET(g_ListenSocket, &ReadSet);
			UserTable_SOCKET[0] = g_ListenSocket;
			iSocketCount = 1;
		}
	}

	//전체 클라이언트 for문 종료 후 socket count 수치가 남아있다면 추가적으로 마지막 select.
	//64개 session이 다 안 차거나, 뒷부분 자투리가 남는 경우
	if (iSocketCount > 0)
	{
		netSelectSocket(UserTable_SOCKET, &ReadSet, &WriteSet);
	}
}

void netSelectSocket(SOCKET * pTableSocket, FD_SET * pReadSet, FD_SET * pWriteSet)
{
	timeval Time;
	int iRet, iCnt;
	bool bProcFlag;

	//timeout을 0으로 하지 않으면 뒷쪽 소켓들은 검사 시간이 늦어진다
	Time.tv_sec = 0;
	Time.tv_usec = 0;

	iRet = select(0, pReadSet, pWriteSet, 0, &Time);

	//리턴 값이 0이상이라면 누군가의 데이터가 왔다 
	if (0 < iRet)
	{
		//table socket을 돌면서 어떤 소켓에 반응이 있었는지 확인
		for (iCnt = 0; iRet > 0 && iCnt < FD_SETSIZE; ++iCnt)
		{
			bProcFlag = true;
			if (pTableSocket[iCnt] == INVALID_SOCKET)
				continue;

			//write 체크
			if (FD_ISSET(pTableSocket[iCnt], pWriteSet))
			{
				--iRet;
				bProcFlag = netSendProc(pTableSocket[iCnt]);
			}

			//read 체크
			if (FD_ISSET(pTableSocket[iCnt], pReadSet))
			{
				--iRet;

				//netSendProc 에서 에러(접속끊김 등)의 상황으로 이미 클라가 접속종료 한 경우 있음
				//bProcFlag 확인 후 Recv 진행
				if (bProcFlag)
				{
					//listen socket은 접속자 수락용도 별도 처리
					if (pTableSocket[iCnt] == g_ListenSocket)
						netAcceptProc();
					else
						netRecvProc(pTableSocket[iCnt]);
				}
			}
		}
	}
	else if (iRet == SOCKET_ERROR)
	{
		Log(L"select() error!!! \n", dfLOG_LEVEL_DEBUG);
	}
}

void netRecvProc(SOCKET socket)
{
	st_SESSION *pSession;
	int iBufferSize;
	int iRet;

	pSession = FindSession(socket);

	if (NULL == pSession)
		return;

	//마지막으로 받은 메시지 타임
	pSession->dwRecvTime = timeGetTime();
	pSession->dwRecvCount++;
	pSession->dwRecvSecondTick = pSession->dwRecvTime - pSession->dwRecvSecondTick;


	//받기 작업
	iBufferSize = pSession->RecvQ.DirectEnqueueSize();
	iRet = recv(pSession->socket, pSession->RecvQ.GetRearBufferPtr(), iBufferSize, 0);

	if (SOCKET_ERROR == iRet)// || 0 == iRet)
	{
		wprintf(L"[%d] ", pSession->socket);
		Log(L"socket disconnect \n", dfLOG_LEVEL_DEBUG);
		//DisconnectSession(pSession->socket);
		return;
	}

	//받은 데이터가 있다면
	if (0 < iRet)
	{
		//recvQ에 넣은걸로
		pSession->RecvQ.MoveRear(iRet);

		//패킷이 완료되었는지 확인. 하나 이상이 버퍼에 있을 수 있음. 전부 처리
		while (true)
		{
			iRet = CompleteRecvPacket(pSession);

			if (1 == iRet) // 처리할 패킷 없음
				break;

			if (-1 == iRet) //패킷 처리 오류
			{
				_LOG(dfLOG_LEVEL_ERROR, L"packet recv ERROR session ID : %d", pSession->dwSessionID);
				return;
			}
		}
	}
}

bool netSendProc(SOCKET socket)
{
	//클라에 들어있는 큐 확인
	st_SESSION *session = FindSession(socket);
	RingBuf* packet = &session->SendQ;
	int iBufferSize;
	int iRet;

	if (NULL == session)
		return false;

	//char tempbuf[1000] = { 0, };
	//printf("buffer");
	//session->SendQ.Peek(tempbuf, session->SendQ.GetUseSize());
	//for (int i = 0; i < session->SendQ.GetUseSize(); i++)
	//{
	//	printf(" : %02X", (unsigned char)tempbuf[i]);
	//}
	//printf("\n");

	//send
	iBufferSize = session->SendQ.DirectDequeueSize();
	iRet = send(session->socket, session->SendQ.GetFrontBufferPtr(), iBufferSize, 0);
	session->SendQ.MoveFront(iRet);

	if (iRet > 0)
	{
		//		wprintf(L"[%d] send : %d byte \n", session->socket, sendSize);
	}

	if (iRet <= 0)
		return true;
}

bool netAcceptProc(void)
{
	SOCKADDR_IN addr;
	int addrLen = sizeof(addr);

	SOCKET clientSocket = accept(g_ListenSocket, (SOCKADDR*)&addr, &addrLen);

	//char Ip[20];
	//inet_ntop(AF_INET, &(addr->sin_addr), Ip, INET_ADDRSTRLEN);

	st_SESSION* newSession = new st_SESSION;
	newSession->socket = clientSocket;
	newSession->dwSessionID = g_SessionCount++;
	newSession->dwRecvCount = 0;
	newSession->dwRecvTime = timeGetTime();
	newSession->dwRecvSecondTick = timeGetTime();
	//memcpy(&newSession->ip, &addr.sin_addr, sizeof(addr.sin_addr));
	//newSession->port = addr.sin_port;

	//list에 삽입
	g_SessionMap.insert({ newSession->socket, newSession });

	//wprintf(L"[%d] accpet / total : %d \n", newSession->socket, sessionList.size());

	//캐릭터 생성
	CreateCharacter(newSession);

	return true;
}

int CompleteRecvPacket(st_SESSION * pSession)
{
	st_PACKET_HEADER stPacketHeader;
	int iRecvQSize;

	iRecvQSize = pSession->RecvQ.GetUseSize();

	if (pSession->dwRecvCount >= 3)
	{
		int a = 0;
	}

	//패킷 헤더 크기 이상인 경우에 -> 받은 내용 검사
	if (sizeof(st_PACKET_HEADER) > iRecvQSize)
		return 1;

	//1. packet code 검사
	pSession->RecvQ.Peek((char*)&stPacketHeader, sizeof(st_PACKET_HEADER));
	if (dfPACKET_CODE != stPacketHeader.byCode)
	{
		_LOG(dfLOG_LEVEL_ERROR, L"Packet header code error (code : %x)!!!", stPacketHeader.byCode);
		return -1;
	}

	//2. 큐에 저장된 데이터가 패킷의 크기만큼 있는지 확인. end code가 있으니 +1
	if (stPacketHeader.bySize + sizeof(st_PACKET_HEADER) + 1 > iRecvQSize)
		return 1;

	//3. 위에서 헤더를 peek으로 봤으니 큐에서 삭제
	pSession->RecvQ.RemoveData(sizeof(st_PACKET_HEADER));

	CPacket packet;

	//4. payload 부분 직렬화 버퍼로 빼오기
	if (!pSession->RecvQ.Dequeue(packet.GetBufferPtr(), stPacketHeader.bySize))
	{
		_LOG(dfLOG_LEVEL_ERROR, L"Packet recvQ Dequeue error !!!");
		return -1;
	}

	//5. End code 부분 버퍼로 빼오기
	BYTE byEndCode;
	if (!pSession->RecvQ.Dequeue((char*)&byEndCode, 1))
	{
		_LOG(dfLOG_LEVEL_ERROR, L"Packet recvQ Dequeue end code error !!!");
		return -1;
	}
	if (dfNETWORK_PACKET_END != byEndCode)
	{
		_LOG(dfLOG_LEVEL_ERROR, L"Packet byEndCode error (%x) !!!", byEndCode);
		return -1;
	}

	//6. 패킷 클래스에 버퍼 포인터를 얻어 임의로 데이터를 넣었으므로 클래스 내부의 사이즈 이동처리 다시 해주자
	packet.MoveWritePos(stPacketHeader.bySize);

	//7. 실질적인 패킷 처리 함수를 호출한다 
	if (!PacketProc(pSession, stPacketHeader.byType, &packet))
		return -1;

	return 0;
}

bool PacketProc(st_SESSION * pSession, BYTE byPacketType, CPacket * pPacket)
{
	switch (byPacketType)
	{
	case dfPACKET_CS_MOVE_START:
		return netPacketProc_MoveStart(pSession, pPacket);
		break;
	case dfPACKET_CS_MOVE_STOP:
		return netPacketProc_MoveStop(pSession, pPacket);
		break;
	case dfPACKET_CS_ATTACK1:
		return netPacketProc_Attack1(pSession, pPacket);
		break;
	case dfPACKET_CS_ATTACK2:
		return netPacketProc_Attack2(pSession, pPacket);
		break;
	case dfPACKET_CS_ATTACK3:
		return netPacketProc_Attack3(pSession, pPacket);
		break;
	}

	return true;
}

void SendPacket_Around(st_SESSION * pSession, CPacket * pPacket, bool includePlayer)
{
	st_CHARACTER *character = FindCharacter(pSession->dwSessionID);

	st_SECTOR_AROUND DestSector;
	GetSectorAround(character->CurSector.iX, character->CurSector.iY, &DestSector);

	for (int iCnt = 0; iCnt < DestSector.iCount; iCnt++)
	{
		SendPacket_SectorOne(pSession, pPacket, &DestSector.Around[iCnt]);
	}

	if (includePlayer)
	{
		printf("send include me! \n");
		pSession->SendQ.Enqueue(pPacket->GetReadPtr(), pPacket->GetDataSize());
	}
}

void SendPacket_Unicast(st_SESSION *pSession, CPacket* pPacket)
{
	if (pSession == nullptr)
		return;

	pSession->SendQ.Enqueue(pPacket->GetReadPtr(), pPacket->GetDataSize());
}

void SendPacket_Broadcast(st_SESSION *pSession, CPacket* pPacket)
{
	if (pSession == nullptr)
		return;

	std::unordered_map<DWORD, st_CHARACTER*>::iterator iter = g_CharacterMap.begin();
	std::unordered_map<DWORD, st_CHARACTER*>::iterator endIter = g_CharacterMap.end();

	for (iter; iter != endIter; ++iter)
	{
		st_SESSION* session = iter->second->pSession;
		if (session != pSession)
			session->SendQ.Enqueue(pPacket->GetReadPtr(), pPacket->GetDataSize());
	}
}

void SendPacket_SectorOne(st_SESSION *pSession, CPacket *pPacket, st_SECTOR_POS *pPos)
{
	if (pPos == nullptr)
		return;

	if (pPos->iX <= -1 || pPos->iX > dfSECTOR_MAX_X ||
		pPos->iY <= -1 || pPos->iY > dfSECTOR_MAX_Y)
		return;

	list<st_CHARACTER *> *list = &g_Sector[pPos->iY][pPos->iX];

	auto begin = list->begin();
	auto end = list->end();

	for (auto iter = begin; iter != end; ++iter)
	{
		st_CHARACTER *character = (*iter);

		if (character->pSession != pSession)
			character->pSession->SendQ.Enqueue(pPacket->GetReadPtr(), pPacket->GetDataSize());
	}

	//if (includePlayer)
	//{
	//	printf("send include me! \n");
	//	pSession->SendQ.Enqueue(pPacket->GetReadPtr(), pPacket->GetDataSize());
	//}
}
