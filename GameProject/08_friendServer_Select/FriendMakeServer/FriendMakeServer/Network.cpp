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

	//list�� ����
	sessionList.push_back(newSession);

}

void CNetwork::networkFunc()
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

		timeval time;
		time.tv_usec = 0;
		time.tv_sec = 0;

		int ret = select(0, &ReadSet, &WriteSet, 0, &time); //������ timeout�� 0���� �־���Ѵ�. ���� ���⼭ ���� �� �ڿ��ִ� select�� ��� ��ٸ��Ե�. ��� ������ ��û ���Եȴ�.
		
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

			SESSION *session = *startIter;
			++startIter;

			//recv üũ
			if (FD_ISSET(session->socket, &ReadSet))
			{
				recvCheck(session);
			}

			if (session != nullptr)
			{
				//send üũ
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
	//Ŭ�� ����ִ� ť Ȯ��
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
		//���ۿ��� ������ ������ ���� �� ����������� ó��.
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
				//ȸ������ ��û
			case df_REQ_ACCOUNT_ADD:
				recv_AccountAdd_Require(session, wPayloadSize);
				break;

				//ȸ�� �α���
			case df_REQ_LOGIN:
				recv_Login_Require(session, wPayloadSize);
				break;

				//ȸ�� ����Ʈ ��û
			case df_REQ_ACCOUNT_LIST:
				break;

				//ģ�� ��� ��û
			case df_REQ_FRIEND_LIST:
				break;

				//ģ����û ���� ��� ��û
			case df_REQ_FRIEND_REQUEST_LIST:
				break;

				//ģ�� ��û ������ ��� ��û
			case df_REQ_FRIEND_REPLY_LIST:
				break;

				//ģ�� ���� ���� 
			case df_REQ_FRIEND_REMOVE:
				break;

				//ģ�� ��û
			case df_REQ_FRIEND_REQUEST:
				break;

				//ģ�� ��û ���
			case df_REQ_FRIEND_CANCEL:
				break;

				//ģ�� ��û �ź�
			case df_REQ_FRIEND_DENY:
				break;

				//ģ�� ��û ����
			case df_REQ_FRIEND_AGREE:
				break;

				//��Ʈ���� �׽�Ʈ�� ����
			case df_REQ_STRESS_ECHO:
				break;
			}
		}
	}

	return true;
}

void CNetwork::sendCheck(SESSION *session)
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
	
	//���⼭ �̷��� �ϸ� �α��� �ϰ� ���� �� ������ �ٲ�Ŷ� ����� �ϸ� �ȵ�
	session->userNO = g_userNoCount++;
	wcscpy(session->nickName, nickname);

	//map�� account no�� nickname �߰�
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
		//�ִ� ������ȣ�� �� session�� ������ȣ�� �г����� �ֱ�
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
	// �濡 �� �ִٸ�
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
					// �� ����
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

//�� MsgType, Payload �� �� ����Ʈ ���ϱ� % 256
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