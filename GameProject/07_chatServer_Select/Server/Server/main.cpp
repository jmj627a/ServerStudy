#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <list>
#include <unordered_map> //방 번호와 방 이름, 유저 번호와 유저 닉네임
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
	int m_iRoom_No;					//방 번호
	int m_iRoom_length;				//방 이름 길이
	char* m_cpRoom_name;			//방 이름
	int m_iEnter_player_num;		//방 접속중인 사람 수

};

std::list<SESSION*> sessionList;

int g_playerID = 0;

char buf[dfBUF_SIZE];

std::unordered_map<int, ROOM> roomList;
//std::unordered_map<int, USER> userList;


//첫번째 인자 빼고 전체에게 뿌리기. 
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

//첫번째 인자에게 보내기.
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

	//새로 접속한 클라에게 ID 알려주기
	_MSG msg;
	msg.type = ID_SET_TYPE;
	msg.id = newSession->player_id;
	msg.x = newSession->player_x;
	msg.y = newSession->player_y;
	sendPacket_unicast(newSession, (char*)&msg);


	//새로 접속한 클라에게 남들 위치를 알려주기 
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

	//list에 삽입
	sessionList.push_back(newSession);

	//모든 클라에게 새로 접속한 클라 생성 뿌리기 (나 포함)
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

	//전체를 순회하면서 세션 세팅.
	std::list<SESSION*>::iterator iter;
	for (iter = sessionList.begin(); iter != sessionList.end(); iter++)
	{
		FD_SET((*iter)->socket, &ReadSet);
	}

	//timeval time;
	//time.tv_usec = 50000;

	int ret = select(0, &ReadSet, NULL, NULL, NULL); //timeval을 무한으로 하는 이유 -> 서버는 새로운 일이 없으면 아무것도 할 일이 없다.
	if (ret > 0)
	{
		//listenSocket이 반응하면 accept하기
		if (FD_ISSET(listenSocket, &ReadSet))
		{
			serverAccept(&listenSocket);
		}

		//세션 리스트 돌면서 FD_ISSET으로 니가 반응한 애냐고 물어보고, 걔네 대상으로 recv 시도.
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
					//버퍼에서 받은거 16바이트씩 꺼내서 버퍼 다 비워질때까지 처리.
					_MSG* msg;
					char* ptr;
					ptr = buf;
					int count = 0;

					while (true)
					{
						//msg에다가 꺼내보고 type 분류하기.
						msg = (_MSG*)ptr;

						switch (msg->type)
						{
						case 3: //이동
							MoveStar(*iter, msg->id, msg->x, msg->y);
							break;
						}

						count += sizeof(_MSG);
						ptr += sizeof(_MSG);

						//recv에 쌓인 만큼 데이터를 다 읽었거나, 버퍼 사이즈인 1600이 넘었다면 그만읽기
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
	// 소켓 라이브러리 초기화
	ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret != NO_ERROR) {
		wprintf(L"WSAStartup failed with error: %ld\n", ret);
		return -1;
	}

	// 소켓 생성
	serverSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (serverSocket == INVALID_SOCKET)
		return -1;

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(dfNETWORK_PORT);

	// IP 주소 PORT번호 할당
	ret = bind(serverSocket, (SOCKADDR*)&servAddr, sizeof(servAddr));
	if (ret == SOCKET_ERROR)
		return -1;

	// 생성한 소켓을 서버 소켓으로 완성
	ret = listen(serverSocket, SOMAXCONN);
	if (ret == SOCKET_ERROR)
		return -1;


	while (true)
	{
		network(serverSocket);
	}


	closesocket(serverSocket);

	// 종료전 초기화한 소켓 라이브러리 해제.
	WSACleanup();
	return 0;
}

