#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <list>
#include <unordered_map> //�� ��ȣ�� �� �̸�, ���� ��ȣ�� ���� �г���
#include "Protocol.h"

struct SESSION
{
	SOCKET socket;
	char ip[20];
	int port;

	int userNO;

	eUSER_STATE state;
	int roomNO;
};

struct ROOM
{
	int m_iRoom_No;					//�� ��ȣ
	int m_iRoom_length;				//�� �̸� ����
	char* m_cpRoom_name;			//�� �̸�
	int m_iEnter_player_num;		//�� �������� ��� ��

};

std::list<SESSION*> sessionList;

int g_playerID = 0;

char buf[dfBUF_SIZE];

std::unordered_map<int, ROOM> roomList;
//std::unordered_map<int, USER> userList;


//ù��° ���� ���� ��ü���� �Ѹ���. 
void sendPacket_broadcast(SESSION* session, char* buf)
{
	int ret;
	std::list<SESSION*>::iterator iter;
	for (iter = sessionList.begin(); iter != sessionList.end(); iter++)
	{
		if (session != NULL)
		{
			if ((*iter) == session)
				continue;
		}

		ret = send((*iter)->socket, buf, sizeof(_MSG), 0);

	}
}

//ù��° ���ڿ��� ������.
void sendPacket_unicast(SESSION* session, char* buf)
{
	int ret = send(session->socket, buf, sizeof(_MSG), 0);
}


void serverAccept(SOCKET* listenSocket)
{
	SOCKADDR_IN addr;
	int addrLen = sizeof(addr);

	SOCKET clientSocket = accept(*listenSocket, (SOCKADDR*)&addr, &addrLen);

	//char Ip[20];
	//inet_ntop(AF_INET, &(addr->sin_addr), Ip, INET_ADDRSTRLEN);

	SESSION* newSession = new SESSION;
	newSession->socket = clientSocket;
	memcpy(&newSession->ip, &addr.sin_addr, sizeof(addr.sin_addr));
	newSession->port = addr.sin_port;

	//���� ������ Ŭ�󿡰� ID �˷��ֱ�
	_MSG msg;
	msg.type = ID_SET_TYPE;
	msg.id = newSession->player_id;
	msg.x = newSession->player_x;
	msg.y = newSession->player_y;
	sendPacket_unicast(newSession, (char*)&msg);


	//���� ������ Ŭ�󿡰� ���� ��ġ�� �˷��ֱ� 
	std::list<SESSION*>::iterator iter;
	for (iter = sessionList.begin(); iter != sessionList.end(); iter++)
	{
		_MSG msg;
		msg.type = STAR_SET_TYPE;
		msg.id = (*iter)->player_id;
		msg.x = (*iter)->player_x;
		msg.y = (*iter)->player_y;
		sendPacket_unicast(newSession, (char*)&msg);
	}

	//list�� ����
	sessionList.push_back(newSession);

	//��� Ŭ�󿡰� ���� ������ Ŭ�� ���� �Ѹ��� (�� ����)
	msg.type = STAR_SET_TYPE;
	msg.id = newSession->player_id;
	msg.x = newSession->player_x;
	msg.y = newSession->player_y;
	sendPacket_broadcast(NULL, (char*)&msg);
}

//void MoveStar(SESSION* session, int id, int x, int y)
//{
//	session->player_x = x;
//	session->player_y = y;
//
//	_MSG msg;
//	msg.id = id;
//	msg.type = STAR_MOVE_TYPE;
//	msg.x = x;
//	msg.y = y;
//	sendPacket_broadcast(session, (char*)&msg);
//}
//
//std::list<SESSION*>::iterator Disconnect(SESSION* session)
//{
//	closesocket(session->socket);
//
//	_MSG msg;
//	msg.type = STAR_DELETE_TYPE;
//	msg.id = session->player_id;
//	msg.x = session->player_x;
//	msg.y = session->player_y;
//
//	sendPacket_broadcast(session, (char*)&msg);
//
//	std::list<SESSION*>::iterator iter;
//	iter = std::find(sessionList.begin(), sessionList.end(), session);
//	iter = sessionList.erase(iter);
//	return iter;
//}

void network(SOCKET listenSocket)
{
	FD_SET ReadSet;
	FD_ZERO(&ReadSet);
	FD_SET(listenSocket, &ReadSet);

	//��ü�� ��ȸ�ϸ鼭 ���� ����.
	std::list<SESSION*>::iterator iter;
	for (iter = sessionList.begin(); iter != sessionList.end(); iter++)
	{
		FD_SET((*iter)->socket, &ReadSet);
	}

	//timeval time;
	//time.tv_usec = 50000;

	int ret = select(0, &ReadSet, NULL, NULL, NULL); //timeval�� �������� �ϴ� ���� -> ������ ���ο� ���� ������ �ƹ��͵� �� ���� ����.
	if (ret > 0)
	{
		//listenSocket�� �����ϸ� accept�ϱ�
		if (FD_ISSET(listenSocket, &ReadSet))
		{
			serverAccept(&listenSocket);
		}

		//���� ����Ʈ ���鼭 FD_ISSET���� �ϰ� ������ �ֳİ� �����, �³� ������� recv �õ�.
		for (iter = sessionList.begin(); iter != sessionList.end();)
		{
			if (FD_ISSET((*iter)->socket, &ReadSet))
			{
				//recv 
				int recvSize = recv((*iter)->socket, buf, sizeof(buf), 0);
				if (recvSize == -1)
				{
					iter = Disconnect((*iter));
					continue;
				}
				else if (recvSize > 0)
				{
					//���ۿ��� ������ 16����Ʈ�� ������ ���� �� ����������� ó��.
					_MSG* msg;
					char* ptr;
					ptr = buf;
					int count = 0;

					while (true)
					{
						//msg���ٰ� �������� type �з��ϱ�.
						msg = (_MSG*)ptr;

						switch (msg->type)
						{
						case 3: //�̵�
							MoveStar(*iter, msg->id, msg->x, msg->y);
							break;
						}

						count += sizeof(_MSG);
						ptr += sizeof(_MSG);

						//recv�� ���� ��ŭ �����͸� �� �о��ų�, ���� �������� 1600�� �Ѿ��ٸ� �׸��б�
						if (count == recvSize || count > dfBUF_SIZE)
						{
							memset(buf, 0, sizeof(buf));
							break;
						}
					}
				}
			}

			++iter;
		}
	}
}


int main()
{
	WSADATA wsaData;
	SOCKET serverSocket;
	SOCKADDR_IN servAddr;

	int ret;
	// ���� ���̺귯�� �ʱ�ȭ
	ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret != NO_ERROR) {
		wprintf(L"WSAStartup failed with error: %ld\n", ret);
		return -1;
	}

	// ���� ����
	serverSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (serverSocket == INVALID_SOCKET)
		return -1;

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(dfNETWORK_PORT);

	// IP �ּ� PORT��ȣ �Ҵ�
	ret = bind(serverSocket, (SOCKADDR*)&servAddr, sizeof(servAddr));
	if (ret == SOCKET_ERROR)
		return -1;

	// ������ ������ ���� �������� �ϼ�
	ret = listen(serverSocket, SOMAXCONN);
	if (ret == SOCKET_ERROR)
		return -1;


	while (true)
	{
		network(serverSocket);
	}


	closesocket(serverSocket);

	// ������ �ʱ�ȭ�� ���� ���̺귯�� ����.
	WSACleanup();
	return 0;
}

