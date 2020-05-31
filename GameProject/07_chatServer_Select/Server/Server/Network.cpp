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
	// ���� ���̺귯�� �ʱ�ȭ
	ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret != NO_ERROR) {
		wprintf(L"WSAStartup failed with error: %ld\n", ret);
		return -1;
	}

	// ���� ����
	g_listenSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (g_listenSocket == INVALID_SOCKET)
		return -1;

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(dfNETWORK_PORT);

	// IP �ּ� PORT��ȣ �Ҵ�
	ret = bind(g_listenSocket, (SOCKADDR*)&servAddr, sizeof(servAddr));
	if (ret == SOCKET_ERROR)
		return -1;

	// ������ ������ ���� �������� �ϼ�
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

	//list�� ����
	sessionList.push_back(newSession);

}

void networkFunc()
{
	FD_SET ReadSet;
	FD_SET WriteSet;

	//������ ��ü�� ��ȸ�� iter
	std::list<SESSION*>::iterator iter = sessionList.begin();
	std::list<SESSION*>::iterator startIter;

	while (true)
	{
		//����ؼ� ó������ ��ȸ
		if (iter == sessionList.end())
			iter = sessionList.begin();

		FD_ZERO(&ReadSet);
		FD_ZERO(&WriteSet);
		FD_SET(g_listenSocket, &ReadSet);

		startIter = iter;

		//��ü�� ��ȸ�ϸ鼭 ���� ����.
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

		int ret = select(0, &ReadSet, &WriteSet, NULL, NULL); //timeval�� �������� �ϴ� ���� -> ������ ���ο� ���� ������ �ƹ��͵� �� ���� ����.
		
		if (ret == SOCKET_ERROR)
			return;

		//listenSocket�� �����ϸ� accept�ϱ�
		if (FD_ISSET(g_listenSocket, &ReadSet))
		{
			networkAccept(&g_listenSocket);
		}

		//���� ����Ʈ ���鼭 FD_ISSET���� �ϰ� ������ �ֳİ� �����, �³� ������� recv �õ�.
		for (int i = 0; i < FD_SETSIZE; ++i)
		{
			if (startIter == sessionList.end())
				break;

			//recv üũ
			if (FD_ISSET((*startIter)->socket, &ReadSet))
			{
				recvCheck((*startIter));
			}

			//send üũ
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
	//Ŭ�� ����ִ� ť Ȯ��
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
		//���ۿ��� ������ ������ ���� �� ����������� ó��.
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
				//��ȭ�� ����Ʈ
				break;
			case df_REQ_ROOM_CREATE:
				//��ȭ�� ����
				break;
			case df_REQ_ROOM_ENTER:
				//��ȭ�� ����
				break;
			case df_REQ_CHAT:
				//ä�ü۽�
				break;
			case df_REQ_ROOM_LEAVE:
				break;
			}
		}
	}
}

void sendCheck(SESSION *session)
{
	//Ŭ�� ����ִ� ť Ȯ��
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
	
	//�α��� �� �� �ִ� ���°� �ƴϸ� ����
	if (session->state != eREADY)
		return false;

	//üũ���� ���� ������ ����
	if (CheckSumCheck(df_REQ_LOGIN, (char*)packet->GetReadPtr(), wPayloadSize) != byCheckSum)
	{
		//�α��� ���� send
		sendLoginResponse(session, df_RESULT_LOGIN_ETC);
		wprintf(L"[%d] login fail : df_RESULT_LOGIN_ETC \n", session->userNO);
		return false;
	}

	if (packet->GetDataSize() < wPayloadSize)
		return false;

	WCHAR nickname[16];
	packet->GetData((char *)nickname, wPayloadSize);

	//�ߺ�üũ
	std::list<SESSION*>::iterator iter = sessionList.begin();
	std::list<SESSION*>::iterator end = sessionList.end();

	for (iter; iter != end; ++iter)
	{
		//�α��� ���� �г��� ���� ���� �ǳ� �ٱ� 
		if ((*iter)->state == eREADY)
			continue;

		//�ߺ� �г���
		if (wcscmp((*iter)->nickName, nickname) == 0)
		{
			//�α��� ���� send
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

	//��� ������ �־�� �ϴµ� ���� �����ϱ� ���̷ε尡 ��� üũ���� ����
	CPacket temp;
	temp << result << session->userNO;

	BYTE	byCode = dfPACKET_CODE;
	WORD	wMsgType = df_RES_LOGIN;
	WORD	wPayloadSize = temp.GetDataSize();
	BYTE	byCheckSum = CheckSumCheck(df_RES_LOGIN, temp.GetReadPtr(), wPayloadSize);

	*packet << byCode << byCheckSum << wMsgType << wPayloadSize;
	packet->PutData(temp.GetReadPtr(), wPayloadSize);
}


//�� MsgType, Payload �� �� ����Ʈ ���ϱ� % 256
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