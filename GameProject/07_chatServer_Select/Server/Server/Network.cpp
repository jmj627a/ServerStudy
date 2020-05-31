#include "Network.h"

std::list<SESSION*> sessionList;
std::list<ROOM*> roomList;

SOCKET g_listenSocket;

int userNoCount = 0;

int networkInit()
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
}

void networkAccept(SOCKET* listenSocket)
{
	SOCKADDR_IN addr;
	int addrLen = sizeof(addr);

	SOCKET clientSocket = accept(*listenSocket, (SOCKADDR*)&addr, &addrLen);

	//char Ip[20];
	//inet_ntop(AF_INET, &(addr->sin_addr), Ip, INET_ADDRSTRLEN);

	SESSION* newSession = new SESSION;
	newSession->socket = clientSocket;
	newSession->userNO = userNoCount++;
	newSession->state = eREADY;
	//memcpy(&newSession->ip, &addr.sin_addr, sizeof(addr.sin_addr));
	//newSession->port = addr.sin_port;

	//list에 삽입
	sessionList.push_back(newSession);

}

void networkFunc()
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

		//timeval time;
		//time.tv_usec = 50000;

		int ret = select(0, &ReadSet, &WriteSet, NULL, NULL); //timeval을 무한으로 하는 이유 -> 서버는 새로운 일이 없으면 아무것도 할 일이 없다.
		
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

			//recv 체크
			if (FD_ISSET((*startIter)->socket, &ReadSet))
			{
				recvCheck((*startIter));
			}

			//send 체크
			if (FD_ISSET((*startIter)->socket, &WriteSet))
			{
				sendCheck((*startIter));
			}

			++startIter;
		}
	}
}

bool recvCheck(SESSION *session)
{
	//클라에 들어있는 큐 확인
	CPacket* packet = &session->recvPacket;

	//recv 
	int recvSize = recv(session->socket, packet->GetWritePtr(), packet->GetBufferSize() - packet->GetDataSize(), 0);
	packet->MoveWritePos(recvSize);

	if (recvSize <=0)
	{
		//iter = Disconnect((*iter));
		//continue;
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
			BYTE byCheckSum;
			WORD wMsgType;
			WORD wPayloadSize;

			*packet >> byCode >> byCheckSum >> wMsgType >> wPayloadSize;

			if (byCode != dfPACKET_CODE)
				return false;

			switch (wMsgType)
			{
			case df_REQ_LOGIN:
				recvLoginRequire(session, byCheckSum, wPayloadSize);
				break;
			case df_REQ_ROOM_LIST:
				//대화방 리스트
				break;
			case df_REQ_ROOM_CREATE:
				//대화방 생성
				break;
			case df_REQ_ROOM_ENTER:
				//대화방 입장
				break;
			case df_REQ_CHAT:
				//채팅송신
				break;
			case df_REQ_ROOM_LEAVE:
				break;
			}
		}
	}
}

void sendCheck(SESSION *session)
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

bool recvLoginRequire(SESSION * session, char byCheckSum, WORD wPayloadSize)
{
	CPacket *packet = &session->recvPacket;
	
	//로그인 할 수 있는 상태가 아니면 리턴
	if (session->state != eREADY)
		return false;

	//체크썸이 같지 않으면 리턴
	if (CheckSumCheck(df_REQ_LOGIN, (char*)packet->GetReadPtr(), wPayloadSize) != byCheckSum)
	{
		//로그인 실패 send
		sendLoginResponse(session, df_RESULT_LOGIN_ETC);
		wprintf(L"[%d] login fail : df_RESULT_LOGIN_ETC \n", session->userNO);
		return false;
	}

	if (packet->GetDataSize() < wPayloadSize)
		return false;

	WCHAR nickname[16];
	packet->GetData((char *)nickname, wPayloadSize);

	//중복체크
	std::list<SESSION*>::iterator iter = sessionList.begin();
	std::list<SESSION*>::iterator end = sessionList.end();

	for (iter; iter != end; ++iter)
	{
		//로그인 전에 닉네임 없는 세션 건너 뛰기 
		if ((*iter)->state == eREADY)
			continue;

		//중복 닉네임
		if (wcscmp((*iter)->nickName, nickname) == 0)
		{
			//로그인 실패 send
			sendLoginResponse(session, df_RESULT_LOGIN_DNICK);
			wprintf(L"[%d] login fail : df_RESULT_LOGIN_DNICK \n", session->userNO);
			return false;
		}
	}


	session->state = eLOGIN;
	wcscpy(session->nickName, nickname);


	sendLoginResponse(session, df_RESULT_LOGIN_OK);
	wprintf(L"[%d] login succ : df_RESULT_LOGIN_OK \n", session->userNO);

	return true;
}

void sendLoginResponse(SESSION * session, char result)
{
	CPacket* packet = &session->sendPacket;

	//헤더 다음에 넣어야 하는데 먼저 넣으니까 페이로드가 없어서 체크썸을 못함
	CPacket temp;
	temp << result << session->userNO;

	BYTE	byCode = dfPACKET_CODE;
	WORD	wMsgType = df_RES_LOGIN;
	WORD	wPayloadSize = temp.GetDataSize();
	BYTE	byCheckSum = CheckSumCheck(df_RES_LOGIN, temp.GetReadPtr(), wPayloadSize);

	*packet << byCode << byCheckSum << wMsgType << wPayloadSize;
	packet->PutData(temp.GetReadPtr(), wPayloadSize);
}


//각 MsgType, Payload 의 각 바이트 더하기 % 256
char CheckSumCheck(WORD _type, char* _bufPtr, int _payLoadSize)
{
	char checkSum;

	checkSum = char(((_type) >> 8) & 0xff) + char((_type) & 0xff);

	//_bufPtr += sizeof(st_PACKET_HEADER);

	for (int i = 0; i < _payLoadSize; i++)
		checkSum += _bufPtr[i];

	checkSum %= 256;

	return checkSum;
}