#include "Network.h"

std::list<SESSION*> sessionList;
std::unordered_map<UINT64, WCHAR*> clientInfoMap; //accountNo , NickName

SOCKET g_listenSocket;

UINT64 g_userNoCount = 1;

int CNetwork::networkInit()
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
	g_listenSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (g_listenSocket == INVALID_SOCKET)
		return -1;

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(dfNETWORK_PORT);

	// IP 주소 PORT번호 할당
	ret = bind(g_listenSocket, (SOCKADDR*)&servAddr, sizeof(servAddr));
	if (ret == SOCKET_ERROR)
		return -1;

	// 생성한 소켓을 서버 소켓으로 완성
	ret = listen(g_listenSocket, SOMAXCONN);
	if (ret == SOCKET_ERROR)
		return -1;


	u_long on = 1;
	ioctlsocket(g_listenSocket, FIONBIO, &on);

	return 1;
}

void CNetwork::networkAccept(SOCKET* listenSocket)
{
	SOCKADDR_IN addr;
	int addrLen = sizeof(addr);

	SOCKET clientSocket = accept(*listenSocket, (SOCKADDR*)&addr, &addrLen);

	//char Ip[20];
	//inet_ntop(AF_INET, &(addr->sin_addr), Ip, INET_ADDRSTRLEN);

	SESSION* newSession = new SESSION;
	newSession->socket = clientSocket;

	//memcpy(&newSession->ip, &addr.sin_addr, sizeof(addr.sin_addr));
	//newSession->port = addr.sin_port;

	//list에 삽입
	sessionList.push_back(newSession);

}

void CNetwork::networkFunc()
{
	FD_SET ReadSet;
	FD_SET WriteSet;

	//접속자 전체를 순회할 iter
	std::list<SESSION*>::iterator iter = sessionList.begin();
	std::list<SESSION*>::iterator startIter;

	while (true)
	{
		//계속해서 처음부터 순회
		if (iter == sessionList.end())
			iter = sessionList.begin();

		FD_ZERO(&ReadSet);
		FD_ZERO(&WriteSet);
		FD_SET(g_listenSocket, &ReadSet);

		startIter = iter;

		//전체를 순회하면서 세션 세팅.
		for (int i = 0; i < FD_SETSIZE - 1; ++i) 
		{
			if (sessionList.end() == iter)
				break;

			FD_SET((*iter)->socket, &ReadSet);

			FD_SET((*iter)->socket, &WriteSet);

			iter++;
		}

		timeval time;
		time.tv_usec = 0;
		time.tv_sec = 0;

		int ret = select(0, &ReadSet, &WriteSet, 0, &time); //지금은 timeout을 0으로 넣어야한다. 지금 여기서 잠들면 저 뒤에있는 select는 계속 기다리게됨. 대신 루프가 엄청 돌게된다.
		
		if (ret == SOCKET_ERROR)
			return;

		//listenSocket이 반응하면 accept하기
		if (FD_ISSET(g_listenSocket, &ReadSet))
		{
			networkAccept(&g_listenSocket);
		}

		//세션 리스트 돌면서 FD_ISSET으로 니가 반응한 애냐고 물어보고, 걔네 대상으로 recv 시도.
		for (int i = 0; i < FD_SETSIZE; ++i)
		{
			if (startIter == sessionList.end())
				break;

			SESSION *session = *startIter;
			++startIter;

			//recv 체크
			if (FD_ISSET(session->socket, &ReadSet))
			{
				recvCheck(session);
			}

			if (session != nullptr)
			{
				//send 체크
				if (FD_ISSET(session->socket, &WriteSet))
				{
					sendCheck(session);
				}

			if (sessionList.size() == 0)
				break;
			}
			
		}
	}
}

bool CNetwork::recvCheck(SESSION *session)
{
	//클라에 들어있는 큐 확인
	CPacket* packet = &session->recvPacket;

	//recv 
	int recvSize = recv(session->socket, packet->GetWritePtr(), packet->GetBufferSize() - packet->GetDataSize(), 0);
	packet->MoveWritePos(recvSize);

	if (recvSize <=0)
	{
		//disconnect(session);
		return false;
	}

	if (recvSize > 0)
	{
		//버퍼에서 받은거 꺼내서 버퍼 다 비워질때까지 처리.
		while (true)
		{
			if (packet->GetDataSize() < sizeof(st_PACKET_HEADER))
				return true;

			BYTE byCode;
			WORD wMsgType;
			WORD wPayloadSize;

			*packet >> byCode  >> wMsgType >> wPayloadSize;

			if (byCode != dfPACKET_CODE)
				return true;

			switch (wMsgType)
			{
				//회원가입 요청
			case df_REQ_ACCOUNT_ADD:
				recv_AccountAdd_Require(session, wPayloadSize);
				break;

				//회원 로그인
			case df_REQ_LOGIN:
				recv_Login_Require(session, wPayloadSize);
				break;

				//회원 리스트 요청
			case df_REQ_ACCOUNT_LIST:
				break;

				//친구 목록 요청
			case df_REQ_FRIEND_LIST:
				break;

				//친구요청 보낸 목록 요청
			case df_REQ_FRIEND_REQUEST_LIST:
				break;

				//친구 요청 받은거 목록 요청
			case df_REQ_FRIEND_REPLY_LIST:
				break;

				//친구 관계 끊기 
			case df_REQ_FRIEND_REMOVE:
				break;

				//친구 요청
			case df_REQ_FRIEND_REQUEST:
				break;

				//친구 요청 취소
			case df_REQ_FRIEND_CANCEL:
				break;

				//친구 요청 거부
			case df_REQ_FRIEND_DENY:
				break;

				//친구 요청 수락
			case df_REQ_FRIEND_AGREE:
				break;

				//스트레스 테스트용 에코
			case df_REQ_STRESS_ECHO:
				break;
			}
		}
	}

	return true;
}

void CNetwork::sendCheck(SESSION *session)
{
	//클라에 들어있는 큐 확인
	CPacket* packet = &session->sendPacket;

	//send
	int sendSize = send(session->socket, packet->GetReadPtr(), packet->GetDataSize(), 0);

	if (sendSize > 0)
	{
		session->sendPacket.Clear();
		wprintf(L"[%d] send : %d byte \n", session->userNO, sendSize);
	}

	if (sendSize <=0)
		return;
}

void CNetwork::make_packet(SESSION* session, CPacket* temp, int packet_type)
{
	CPacket* packet = &session->sendPacket;

	BYTE	byCode = dfPACKET_CODE;
	WORD	wMsgType = packet_type;
	WORD	wPayloadSize = temp->GetDataSize();

	*packet << byCode << wMsgType << wPayloadSize;
	packet->PutData(temp->GetReadPtr(), wPayloadSize);
}

SESSION* CNetwork::findAccountNo(UINT64 accountNo)
{
	std::list<SESSION*>::iterator startIter = sessionList.begin();
	std::list<SESSION*>::iterator endIter = sessionList.end();

	for (startIter; startIter != endIter; ++startIter)
	{
		if ((*startIter)->userNO == accountNo)
			return (*startIter);
	}

	return nullptr;
}

WCHAR* CNetwork::findNickname(UINT64 accountNo)
{
	auto findIter = clientInfoMap.find(accountNo);
	if (findIter == clientInfoMap.end())
		return nullptr;
	else
		return findIter->second;
}

bool CNetwork::recv_AccountAdd_Require(SESSION* session, WORD wPayloadSize)
{
	CPacket* packet = &session->recvPacket;

	if (packet->GetDataSize() < wPayloadSize)
		return false;

	WCHAR* nickname = new WCHAR[wPayloadSize * 2];
	packet->GetData((char*)nickname, wPayloadSize);
	
	//여기서 이렇게 하면 로그인 하고 나서 그 정보가 바뀔거라 여기다 하면 안됨
	session->userNO = g_userNoCount++;
	wcscpy(session->nickName, nickname);

	//map에 account no랑 nickname 추가
	clientInfoMap.insert({ session->userNO, nickname });

	send_AccountAdd_Response(session);
	
	wprintf(L"[%d] login succ : df_REQ_ACCOUNT_ADD \n", session->socket);
	return true;
}

bool CNetwork::send_AccountAdd_Response(SESSION* session)
{
	CPacket temp;
	temp << session->userNO;

	make_packet(session, &temp, df_RES_ACCOUNT_ADD);

	return false;
}


bool CNetwork::recv_Login_Require(SESSION * session, WORD wPayloadSize)
{
	CPacket *packet = &session->recvPacket;
	
	if (packet->GetDataSize() < wPayloadSize)
		return false;

	UINT64 accountNo = 1234;
	*packet >> accountNo;

	WCHAR* findname = findNickname(accountNo);

	if (findname != nullptr)
	{
		//있는 계정번호면 이 session에 계정번호와 닉네임을 주기
		session->userNO = accountNo;
		wcscpy(session->nickName, findname);
	}

	send_Login_Response(session, findname);
	wprintf(L"[%d] login succ : df_REQ_LOGIN \n", session->socket);

	return true;
}

bool CNetwork::send_Login_Response(SESSION * session, WCHAR* findName)
{
	CPacket temp;
	if (findName != nullptr)
	{
		temp << session->userNO;
		temp.PutData((char*)findName, dfNICK_MAX_LEN);
	}
	else
		temp << 0;

	make_packet(session, &temp, df_RES_LOGIN);

	return true;
}

/*
void  CNetwork::disconnect(SESSION *session)
{
	// 방에 들어가 있다면
	if (session->state == eROOM)
	{
		std::list<ROOM*>::iterator iter = roomList.begin();

		for (iter; iter != roomList.end(); ++iter)
		{
			ROOM *room = (*iter);

			if (room->m_iRoom_No == session->roomNO)
			{
				//sendRoomExit(session);
				room->m_iEnter_player_num--;

				if (room->m_iEnter_player_num == 0)
				{
					// 방 삭제
					//sendRoomDelete(room);
					roomList.remove(room);
					delete room;
					break;
				}
			}
		}
	}

	sessionList.remove(session);
	closesocket(session->socket);
	delete session;
	session = nullptr;

}

//각 MsgType, Payload 의 각 바이트 더하기 % 256
//char CNetwork::CheckSumCheck(WORD _type, char* _bufPtr, int _payLoadSize)
//{
//	char checkSum;
//
//	checkSum = char(((_type) >> 8) & 0xff) + char((_type) & 0xff);
//
//	for (int i = 0; i < _payLoadSize; i++)
//		checkSum += _bufPtr[i];
//
//	checkSum %= 256;
//
//	return checkSum;
//}

*/