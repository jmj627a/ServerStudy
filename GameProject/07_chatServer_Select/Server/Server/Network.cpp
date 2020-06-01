#include "Network.h"

std::list<SESSION*> sessionList;
std::list<ROOM*> roomList;

SOCKET g_listenSocket;

int userNoCount = 0;
int roomNoCount = 0;

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
	newSession->userNO = userNoCount++;
	newSession->state = eREADY;
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
		disconnect(session);
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
				return true;

			switch (wMsgType)
			{
			case df_REQ_LOGIN:
				recvLoginRequire(session, byCheckSum, wPayloadSize);
				break;
			case df_REQ_ROOM_LIST:
				//��ȭ�� ����Ʈ
				recvRoomListRequire(session, byCheckSum, wPayloadSize);
				break;
			case df_REQ_ROOM_CREATE:
				//��ȭ�� ����
				recvRoomCreateRequire(session, byCheckSum, wPayloadSize);
				break;
			case df_REQ_ROOM_ENTER:
				//��ȭ�� ����
				recvRoomEnterRequire(session, byCheckSum, wPayloadSize);
				break;
			case df_REQ_CHAT:
				//ä�ü۽�
				recvChatRequire(session, byCheckSum, wPayloadSize);
				break;
			case df_REQ_ROOM_LEAVE:
				recvRoomExit(session, byCheckSum, wPayloadSize);
				return true;
				break;
			}
		}
	}

	//session->recvPacket.Clear();
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

bool CNetwork::recvLoginRequire(SESSION * session, char byCheckSum, WORD wPayloadSize)
{
	CPacket *packet = &session->recvPacket;
	
	//�α��� �� �� �ִ� ���°� �ƴϸ� ����
	if (session->state != eREADY)
	{
		return false;
	}

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

void CNetwork::sendLoginResponse(SESSION * session, char result)
{
	CPacket* packet = &session->sendPacket;

	//��� ������ �־�� �ϴµ� ���� �����ϱ� ���̷ε尡 ��� üũ���� ����
	CPacket temp;
	temp << result << session->userNO;

	BYTE	byCode = dfPACKET_CODE;
	WORD	wMsgType = df_RES_LOGIN;
	WORD	wPayloadSize = temp.GetDataSize();
	BYTE	byCheckSum = CheckSumCheck(wMsgType, temp.GetReadPtr(), wPayloadSize);

	*packet << byCode << byCheckSum << wMsgType << wPayloadSize;
	packet->PutData(temp.GetReadPtr(), wPayloadSize);
}

bool CNetwork::recvRoomListRequire(SESSION * session, char byCheckSum, WORD wPayloadSize)
{
	CPacket *packet = &session->recvPacket;

	//�α��� �� �� �ִ� ���°� �ƴϸ� ����
	if (session->state != eLOGIN)
	{
		return false;
	}
	//üũ���� ���� ������ ����
	if (CheckSumCheck(df_REQ_ROOM_LIST, (char*)packet->GetReadPtr(), wPayloadSize) != byCheckSum)
	{
		wprintf(L"[%d] RoomListRequire fail : df_REQ_ROOM_LIST \n", session->userNO);
		return false;
	}

	if (packet->GetDataSize() < wPayloadSize)
		return false;

	session->state = eLOBBY;

	sendRoomList(session);
	wprintf(L"[%d] RoomListRequire succ : df_REQ_ROOM_LIST \n", session->userNO);

	return true;

}

void CNetwork::sendRoomList(SESSION * session)
{
	CPacket* packet = &session->sendPacket;

	//��� ������ �־�� �ϴµ� ���� �����ϱ� ���̷ε尡 ��� üũ���� ����
	CPacket temp;
	WORD roomNum = roomList.size();
	temp << roomNum;

	std::list<ROOM*>::iterator iter = roomList.begin();

	for (int i = 0; i < roomNum; ++i)
	{
		temp << (*iter)->m_iRoom_No << (*iter)->m_iRoom_length;
		temp.PutData((char*)(*iter)->m_cpRoom_name, (*iter)->m_iRoom_length);
		temp << (*iter)->m_iEnter_player_num;

		std::list<SESSION*>::iterator Useriter = sessionList.begin();
		for (int j = 0; j < sessionList.size(); ++j)
		{
			if ((*Useriter)->state == eROOM)
				if ((*Useriter)->roomNO == (*iter)->m_iRoom_No)
					temp.PutData((char*)(*Useriter)->nickName, 30);
			
			Useriter++;
		}

		iter++;
	}


	BYTE	byCode = dfPACKET_CODE;
	WORD	wMsgType = df_RES_ROOM_LIST;
	WORD	wPayloadSize = temp.GetDataSize();
	BYTE	byCheckSum = CheckSumCheck(wMsgType, temp.GetReadPtr(), wPayloadSize);

	*packet << byCode << byCheckSum << wMsgType << wPayloadSize;
	packet->PutData(temp.GetReadPtr(), wPayloadSize);
}

bool CNetwork::recvRoomCreateRequire(SESSION * session, char byCheckSum, WORD wPayloadSize)
{
	CPacket *packet = &session->recvPacket;

	//�� ���� �� �� �ִ� ���°� �ƴϸ� ����
	if (session->state != eLOBBY)
	{
		return false;
	}

	//üũ���� ���� ������ ����
	if (CheckSumCheck(df_REQ_ROOM_CREATE, (char*)packet->GetReadPtr(), wPayloadSize) != byCheckSum)
	{
		sendRoomCreateResonse(session, nullptr, df_RESULT_ROOM_CREATE_ETC);
		wprintf(L"[%d] RoomCreateRequire fail : df_REQ_ROOM_CREATE \n", session->userNO);
		return false;
	}

	if (packet->GetDataSize() < wPayloadSize)
		return false;

	session->state = eLOBBY;

	WORD roomLength = 0;
	*packet >> roomLength;
	WCHAR* roomName = new WCHAR[roomLength/2];
	packet->GetData((char *)roomName, roomLength);
	roomName[roomLength / 2] = '\0';

	//�ߺ�üũ
	std::list<ROOM*>::iterator iter = roomList.begin();
	std::list<ROOM*>::iterator end = roomList.end();

	for (iter; iter != end; ++iter)
	{
		//�ߺ� �� �̸�
		if (wcscmp((*iter)->m_cpRoom_name, roomName) == 0)
		{
			//���� send
			sendRoomCreateResonse(session,nullptr, df_RESULT_ROOM_CREATE_DNICK);
			wprintf(L"[%d] RoomCreate fail : df_RESULT_ROOM_CREATE_DNICK \n", session->userNO);
			return false;
		}
	}

	ROOM* newRoom = new ROOM();
	newRoom->m_iEnter_player_num =0;
	newRoom->m_iRoom_length = roomLength;
	newRoom->m_iRoom_No = roomNoCount++;
	newRoom->m_cpRoom_name = new WCHAR[roomLength / 2];
	wcscpy(newRoom->m_cpRoom_name, roomName);

	roomList.push_back(newRoom);

	sendRoomCreateResonse(session, newRoom, df_RESULT_ROOM_CREATE_OK);
	wprintf(L"[%d] RoomCreate succ : df_RESULT_ROOM_CREATE_OK \n", session->userNO);

	return true;

}

void CNetwork::sendRoomCreateResonse(SESSION * session, ROOM* room, char result)
{

	//��� ������ �־�� �ϴµ� ���� �����ϱ� ���̷ε尡 ��� üũ���� ����
	CPacket temp;
	if (room != nullptr)
	{
		temp << result << room->m_iRoom_No << room->m_iRoom_length;
		temp.PutData((char*)room->m_cpRoom_name, room->m_iRoom_length);
	}

	BYTE	byCode = dfPACKET_CODE;
	WORD	wMsgType = df_RES_ROOM_CREATE;
	WORD	wPayloadSize = temp.GetDataSize();
	BYTE	byCheckSum = CheckSumCheck(wMsgType, temp.GetReadPtr(), wPayloadSize);

	switch (result)
	{
	case df_RESULT_ROOM_CREATE_OK:
	{
		std::list<SESSION*>::iterator iter = sessionList.begin();

		for (iter; iter != sessionList.end(); iter++)
		{
			if ((*iter)->state == eREADY)
				continue;

			CPacket* packet = &(*iter)->sendPacket;
			*packet << byCode << byCheckSum << wMsgType << wPayloadSize;
			packet->PutData(temp.GetReadPtr(), wPayloadSize);

		}
	}
		break;

	default:
	{
		CPacket* packet = &session->sendPacket;
		*packet << byCode << byCheckSum << wMsgType << wPayloadSize;
		packet->PutData(temp.GetReadPtr(), wPayloadSize);
	}
	break;
	}


}

bool CNetwork::recvRoomEnterRequire(SESSION * session, char byCheckSum, WORD wPayloadSize)
{
	CPacket *packet = &session->recvPacket;

	//���� �� �� �ִ� ���°� �ƴϸ� ����
	if (session->state != eLOBBY)
	{
		return false;
	}
	//üũ���� ���� ������ ����
	if (CheckSumCheck(df_REQ_ROOM_ENTER, (char*)packet->GetReadPtr(), wPayloadSize) != byCheckSum)
	{
		//���� send
		sendRoomEnterResonse(session, nullptr, df_RESULT_ROOM_ENTER_ETC);
		wprintf(L"[%d] Room Enter fail : df_RESULT_ROOM_ENTER_ETC \n", session->userNO);
		return false;
	}

	if (packet->GetDataSize() < wPayloadSize)
		return false;

	int roomNo;
	*packet >> roomNo;
	
	std::list<ROOM*>::iterator iter = roomList.begin();
	std::list<ROOM*>::iterator end = roomList.end();

	for (iter; iter != end;)
	{
		if ((*iter)->m_iRoom_No == roomNo)
		{
			session->roomNO = roomNo;
			session->state = eROOM;
			(*iter)->m_iEnter_player_num++;
			break;
		}
		iter++;
	}

	if (iter != end)
	{
		sendRoomEnterResonse(session, *iter, df_RESULT_ROOM_ENTER_OK);
		sendRoomEnterUser(session, *iter);
		wprintf(L"[%d] Room Enter succ : df_RESULT_ROOM_ENTER_OK \n", session->userNO);
	}
	else
	{
		sendRoomEnterResonse(session, nullptr, df_RESULT_ROOM_ENTER_ETC);
		wprintf(L"[%d] Room Enter fail : df_RESULT_ROOM_ENTER_ETC \n", session->userNO);
	}

	return true;
}

void CNetwork::sendRoomEnterResonse(SESSION * session, ROOM* room, char result)
{
	//��� ������ �־�� �ϴµ� ���� �����ϱ� ���̷ε尡 ��� üũ���� ����
	CPacket temp;

	if (result != df_RESULT_ROOM_ENTER_OK)
		return;

	temp << result;
	if (room != nullptr)
	{
		temp << room->m_iRoom_No << room->m_iRoom_length;
		temp.PutData((char*)room->m_cpRoom_name, room->m_iRoom_length);
	}

	std::list<SESSION*>::iterator iter = sessionList.begin();
	std::list<SESSION*>::iterator end = sessionList.end();

	temp << room->m_iEnter_player_num;

	for (iter; iter != end; ++iter)
	{
		if ((*iter)->state == eROOM)
		{
			if ((*iter)->roomNO == room->m_iRoom_No)
			{
				temp.PutData((char*)(*iter)->nickName, 30);
				temp << (*iter)->userNO;
			}
		}
	}

	BYTE	byCode = dfPACKET_CODE;
	WORD	wMsgType = df_RES_ROOM_ENTER;
	WORD	wPayloadSize = temp.GetDataSize();
	BYTE	byCheckSum = CheckSumCheck(wMsgType, temp.GetReadPtr(), wPayloadSize);

	CPacket* packet = &session->sendPacket;
	*packet << byCode << byCheckSum << wMsgType << wPayloadSize;
	packet->PutData(temp.GetReadPtr(), wPayloadSize);
}

void CNetwork::sendRoomEnterUser(SESSION * session, ROOM * room)
{
	//��� ������ �־�� �ϴµ� ���� �����ϱ� ���̷ε尡 ��� üũ���� ����
	CPacket temp;
	temp.PutData((char*)session->nickName, 30);
	temp << session->userNO;

	BYTE	byCode = dfPACKET_CODE;
	WORD	wMsgType = df_RES_USER_ENTER;
	WORD	wPayloadSize = temp.GetDataSize();
	BYTE	byCheckSum = CheckSumCheck(wMsgType, temp.GetReadPtr(), wPayloadSize);

	std::list<SESSION*>::iterator iter = sessionList.begin();
	std::list<SESSION*>::iterator end = sessionList.end();

	for (iter; iter != end; ++iter)
	{
		//���� ���� ������
		if ((*iter)->userNO == session->userNO)
			continue;

		if ((*iter)->state == eROOM)
		{
			if ((*iter)->roomNO == room->m_iRoom_No)
			{
				CPacket* packet = &(*iter)->sendPacket;
				*packet << byCode << byCheckSum << wMsgType << wPayloadSize;
				packet->PutData(temp.GetReadPtr(), wPayloadSize);
			}
		}
	}
}

bool CNetwork::recvChatRequire(SESSION * session, char byCheckSum, WORD wPayloadSize)
{
	CPacket *packet = &session->recvPacket;

	//���� �� �� �ִ� ���°� �ƴϸ� ����
	if (session->state != eROOM)
	{
		return false;
	}
	//üũ���� ���� ������ ����
	if (CheckSumCheck(df_REQ_CHAT, (char*)packet->GetReadPtr(), wPayloadSize) != byCheckSum)
	{
		//���� send
		wprintf(L"[%d] chat send fail : df_REQ_CHAT \n", session->userNO);
		return false;
	}

	if (packet->GetDataSize() < wPayloadSize)
		return false;

	WORD messageSize;
	*packet >> messageSize;

	WCHAR* chatMessage = new WCHAR[messageSize / 2];
	packet->GetData((char *)chatMessage, messageSize);
	chatMessage[messageSize / 2] = '\0';

	sendChatResonse(session, chatMessage, messageSize);
	wprintf(L"[%d] chat send fail : df_REQ_CHAT \n", session->userNO);

	return true;
}

void CNetwork::sendChatResonse(SESSION * session, WCHAR* message, WORD messageSize)
{
	//��� ������ �־�� �ϴµ� ���� �����ϱ� ���̷ε尡 ��� üũ���� ����
	CPacket temp;
	temp << session->userNO << messageSize;
	temp.PutData((char*)message, messageSize);

	BYTE	byCode = dfPACKET_CODE;
	WORD	wMsgType = df_RES_CHAT;
	WORD	wPayloadSize = temp.GetDataSize();
	BYTE	byCheckSum = CheckSumCheck(wMsgType, temp.GetReadPtr(), wPayloadSize);

	std::list<SESSION*>::iterator iter = sessionList.begin();
	std::list<SESSION*>::iterator end = sessionList.end();

	for (iter; iter != end; ++iter)
	{
		//���� ���� ������
		if ((*iter)->userNO == session->userNO)
			continue;

		if ((*iter)->state == eROOM)
		{
			if ((*iter)->roomNO == session->roomNO)
			{
				CPacket* packet = &(*iter)->sendPacket;
				*packet << byCode << byCheckSum << wMsgType << wPayloadSize;
				packet->PutData(temp.GetReadPtr(), wPayloadSize);
			}
		}
	}
}

bool CNetwork::recvRoomExit(SESSION * session, char byCheckSum, WORD wPayloadSize)
{
	CPacket *packet = &session->recvPacket;

	if (session->state != eROOM)
	{
		return false;
	}
	//üũ���� ���� ������ ����
	if (CheckSumCheck(df_REQ_ROOM_LEAVE, (char*)packet->GetReadPtr(), wPayloadSize) != byCheckSum)
	{
		wprintf(L"[%d] Room Exit fail : df_REQ_ROOM_LEAVE \n", session->userNO);
		return false;
	}

	if (packet->GetDataSize() < wPayloadSize)
		return false;

	
	std::list<SESSION*>::iterator iter = sessionList.begin();
	std::list<SESSION*>::iterator end = sessionList.end();

	for (iter; iter != end; ++iter)
	{
		//if ((*iter)->userNO == session->userNO)
		//{
		//	session->state = eLOBBY;
		//	//continue;
		//}
		if ((*iter)->roomNO == session->roomNO)
		{
			sendRoomExit(session);
		}
	}
	std::list<ROOM*>::iterator rIter = roomList.begin();
	std::list<ROOM*>::iterator rEnd = roomList.end();

	ROOM* temp = *rIter;

	for (rIter; rIter != rEnd; ++rIter)
	{
		if ((*rIter)->m_iRoom_No == session->roomNO)
		{
			(*rIter)->m_iEnter_player_num--;
			temp = (*rIter);
			break;
		}
	}


	if (temp->m_iEnter_player_num == 0)
	{
		sendRoomDelete(temp);
		roomList.remove(temp);
		delete temp;
	}

	session->state = eLOBBY;

	wprintf(L"[%d] Room Exit succ : df_REQ_ROOM_LEAVE \n", session->userNO);

	return true;
}

void CNetwork::sendRoomExit(SESSION* session)
{
	//��� ������ �־�� �ϴµ� ���� �����ϱ� ���̷ε尡 ��� üũ���� ����
	CPacket temp;
	temp << session->userNO ;

	BYTE	byCode = dfPACKET_CODE;
	WORD	wMsgType = df_RES_ROOM_LEAVE;
	WORD	wPayloadSize = temp.GetDataSize();
	BYTE	byCheckSum = CheckSumCheck(wMsgType, temp.GetReadPtr(), wPayloadSize);

	std::list<SESSION*>::iterator iter = sessionList.begin();
	std::list<SESSION*>::iterator end = sessionList.end();

	for (iter; iter != end; ++iter)
	{
		if ((*iter)->roomNO == session->roomNO)
		{
			CPacket* packet = &(*iter)->sendPacket;
			*packet << byCode << byCheckSum << wMsgType << wPayloadSize;
			packet->PutData(temp.GetReadPtr(), wPayloadSize);
		}
	}
}

void CNetwork::sendRoomDelete(ROOM* room)
{
	//��� ������ �־�� �ϴµ� ���� �����ϱ� ���̷ε尡 ��� üũ���� ����
	CPacket temp;
	temp << room->m_iRoom_No;

	BYTE	byCode = dfPACKET_CODE;
	WORD	wMsgType = df_RES_ROOM_DELETE;
	WORD	wPayloadSize = temp.GetDataSize();
	BYTE	byCheckSum = CheckSumCheck(wMsgType, temp.GetReadPtr(), wPayloadSize);

	std::list<SESSION*>::iterator iter = sessionList.begin();
	std::list<SESSION*>::iterator end = sessionList.end();

	for (iter; iter != end; ++iter)
	{
		//��ο���
		//if ((*iter)->roomNO == room->m_iRoom_No)
		{
			CPacket* packet = &(*iter)->sendPacket;
			*packet << byCode << byCheckSum << wMsgType << wPayloadSize;
			packet->PutData(temp.GetReadPtr(), wPayloadSize);
		}
	}
}

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
				sendRoomExit(session);
				room->m_iEnter_player_num--;

				if (room->m_iEnter_player_num == 0)
				{
					// �� ����
					sendRoomDelete(room);
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
char CNetwork::CheckSumCheck(WORD _type, char* _bufPtr, int _payLoadSize)
{
	char checkSum;

	checkSum = char(((_type) >> 8) & 0xff) + char((_type) & 0xff);

	for (int i = 0; i < _payLoadSize; i++)
		checkSum += _bufPtr[i];

	checkSum %= 256;

	return checkSum;
}