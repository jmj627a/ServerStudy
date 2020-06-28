#include "Network.h"

//��� ���� ����� �α� ����
extern int g_iLogLevel;

//�α� ����� �ʿ��� �ӽ� ����
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
	// ���� ���̺귯�� �ʱ�ȭ
	ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret != NO_ERROR) {
		wprintf(L"WSAStartup failed with error: %ld\n", ret);
		return -1;
	}

	// ���� ����
	g_ListenSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (g_ListenSocket == INVALID_SOCKET)
		return -1;

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(dfNETWORK_PORT);

	// IP �ּ� PORT��ȣ �Ҵ�
	ret = bind(g_ListenSocket, (SOCKADDR*)&servAddr, sizeof(servAddr));
	if (ret == SOCKET_ERROR)
		return -1;

	// ������ ������ ���� �������� �ϼ�
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

	//FD_SET�� FD_SIZE��ŭ���� ���� �˻簡 �����ϴ�
	//�׷��Ƿ� �� ������ŭ �־ �����
	FD_SET ReadSet;
	FD_SET WriteSet;

	FD_ZERO(&ReadSet);
	FD_ZERO(&WriteSet);

	//���� ���� �ֱ�
	FD_SET(g_ListenSocket, &ReadSet);
	UserTable_SOCKET[iSocketCount] = g_ListenSocket;
	iSocketCount++;

	//���� ���� �� �������� ��� Ŭ���̾�Ʈ�� ���� SOCKET�� üũ�Ѵ�
	std::unordered_map<SOCKET, st_SESSION*>::iterator sessionIter;
	for (sessionIter = g_SessionMap.begin(); sessionIter != g_SessionMap.end(); )
	{
		pSession = sessionIter->second;

		//�ش� Ŭ���̾�Ʈ ReadSet ��� / SendQ�� �����Ͱ� �ִٸ� WriteSet ���
		UserTable_SOCKET[iSocketCount] = pSession->socket;

		FD_SET(pSession->socket, &ReadSet);
		if(pSession->SendQ.GetUseSize() > 0)
			FD_SET(pSession->socket, &WriteSet);

		iSocketCount++;
		sessionIter++;			//�̸� �������ѵּ� �ؿ��� �����Ǿ iter�� ���������

		//select �ִ�ġ ����. ������� ���̺� ������ select �Լ� ȣ�� �� ����
		if (FD_SETSIZE <= iSocketCount)
		{
			netSelectSocket(UserTable_SOCKET, &ReadSet, &WriteSet);

			FD_ZERO(&ReadSet);
			FD_ZERO(&WriteSet);

			memset(UserTable_SOCKET, INVALID_SOCKET, sizeof(SOCKET) * FD_SETSIZE);

			//64������ acceptó���� ���ֱ� ���� listen Socket�� ����
			FD_SET(g_ListenSocket, &ReadSet);
			UserTable_SOCKET[0] = g_ListenSocket;
			iSocketCount = 1;
		}
	}

	//��ü Ŭ���̾�Ʈ for�� ���� �� socket count ��ġ�� �����ִٸ� �߰������� ������ select.
	//64�� session�� �� �� ���ų�, �޺κ� �������� ���� ���
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

	//timeout�� 0���� ���� ������ ���� ���ϵ��� �˻� �ð��� �ʾ�����
	Time.tv_sec = 0;
	Time.tv_usec = 0;

	iRet = select(0, pReadSet, pWriteSet, 0, &Time);

	//���� ���� 0�̻��̶�� �������� �����Ͱ� �Դ� 
	if (0 < iRet)
	{
		//table socket�� ���鼭 � ���Ͽ� ������ �־����� Ȯ��
		for (iCnt = 0; iRet > 0 && iCnt < FD_SETSIZE; ++iCnt)
		{
			bProcFlag = true;
			if (pTableSocket[iCnt] == INVALID_SOCKET)
				continue;

			//write üũ
			if (FD_ISSET(pTableSocket[iCnt], pWriteSet))
			{
				--iRet;
				bProcFlag = netSendProc(pTableSocket[iCnt]);
			}

			//read üũ
			if (FD_ISSET(pTableSocket[iCnt], pReadSet))
			{
				--iRet;

				//netSendProc ���� ����(���Ӳ��� ��)�� ��Ȳ���� �̹� Ŭ�� �������� �� ��� ����
				//bProcFlag Ȯ�� �� Recv ����
				if (bProcFlag)
				{
					//listen socket�� ������ �����뵵 ���� ó��
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

	//���������� ���� �޽��� Ÿ��
	pSession->dwRecvTime = timeGetTime();
	pSession->dwRecvCount++;
	pSession->dwRecvSecondTick = pSession->dwRecvTime - pSession->dwRecvSecondTick;


	//�ޱ� �۾�
	iBufferSize = pSession->RecvQ.DirectEnqueueSize();
	iRet = recv(pSession->socket, pSession->RecvQ.GetRearBufferPtr(), iBufferSize, 0);

	if (SOCKET_ERROR == iRet)// || 0 == iRet)
	{
		wprintf(L"[%d] ", pSession->socket);
		Log(L"socket disconnect \n", dfLOG_LEVEL_DEBUG);
		//DisconnectSession(pSession->socket);
		return;
	}

	//���� �����Ͱ� �ִٸ�
	if (0 < iRet)
	{
		//recvQ�� �����ɷ�
		pSession->RecvQ.MoveRear(iRet);

		//��Ŷ�� �Ϸ�Ǿ����� Ȯ��. �ϳ� �̻��� ���ۿ� ���� �� ����. ���� ó��
		while (true)
		{
			iRet = CompleteRecvPacket(pSession);

			if (1 == iRet) // ó���� ��Ŷ ����
				break;

			if (-1 == iRet) //��Ŷ ó�� ����
			{
				_LOG(dfLOG_LEVEL_ERROR, L"packet recv ERROR session ID : %d", pSession->dwSessionID);
				return;
			}
		}
	}
}

bool netSendProc(SOCKET socket)
{
	//Ŭ�� ����ִ� ť Ȯ��
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

	//list�� ����
	g_SessionMap.insert({ newSession->socket, newSession });

	//wprintf(L"[%d] accpet / total : %d \n", newSession->socket, sessionList.size());

	//ĳ���� ����
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

	//��Ŷ ��� ũ�� �̻��� ��쿡 -> ���� ���� �˻�
	if (sizeof(st_PACKET_HEADER) > iRecvQSize)
		return 1;

	//1. packet code �˻�
	pSession->RecvQ.Peek((char*)&stPacketHeader, sizeof(st_PACKET_HEADER));
	if (dfPACKET_CODE != stPacketHeader.byCode)
	{
		_LOG(dfLOG_LEVEL_ERROR, L"Packet header code error (code : %x)!!!", stPacketHeader.byCode);
		return -1;
	}

	//2. ť�� ����� �����Ͱ� ��Ŷ�� ũ�⸸ŭ �ִ��� Ȯ��. end code�� ������ +1
	if (stPacketHeader.bySize + sizeof(st_PACKET_HEADER) + 1 > iRecvQSize)
		return 1;

	//3. ������ ����� peek���� ������ ť���� ����
	pSession->RecvQ.RemoveData(sizeof(st_PACKET_HEADER));

	CPacket packet;

	//4. payload �κ� ����ȭ ���۷� ������
	if (!pSession->RecvQ.Dequeue(packet.GetBufferPtr(), stPacketHeader.bySize))
	{
		_LOG(dfLOG_LEVEL_ERROR, L"Packet recvQ Dequeue error !!!");
		return -1;
	}

	//5. End code �κ� ���۷� ������
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

	//6. ��Ŷ Ŭ������ ���� �����͸� ��� ���Ƿ� �����͸� �־����Ƿ� Ŭ���� ������ ������ �̵�ó�� �ٽ� ������
	packet.MoveWritePos(stPacketHeader.bySize);

	//7. �������� ��Ŷ ó�� �Լ��� ȣ���Ѵ� 
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
