#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <list>

#define SERVERPORT 3000
#define BUFSIZE    1600

#define ID_SET_TYPE			0	 
#define STAR_SET_TYPE		1
#define STAR_DELETE_TYPE	2
#define STAR_MOVE_TYPE		3


struct _MSG
{
	int type;
	int id;
	int x;
	int y;
};


struct SESSION
{
	SOCKET socket;
	char ip[20];
	int port;

	int player_id;
	int player_x;
	int player_y;
};

std::list<SESSION*> sessionList;

int g_playerID = 0;

//첫번째 인자 빼고 전체에게 뿌리기. 
void sendPacket_broadcast(SESSION* session, char* buf)
{
	int ret;
	std::list<SESSION*>::iterator iter;
	for (iter = sessionList.begin(); iter != sessionList.end(); ++iter)
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


void serverAccept(SOCKET listenSocket)
{
	SOCKADDR_IN* addr;
	int addrLen;

	SOCKET clientSocket = accept(listenSocket, (SOCKADDR*)&addr, &addrLen);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
	
	char Ip[20];
	inet_ntop(AF_INET, &(addr->sin_addr), Ip, INET_ADDRSTRLEN);

	SESSION newSession;
	newSession.socket = clientSocket;
	memcpy(&newSession.ip, &Ip, sizeof(Ip));
	newSession.port = addr->sin_port;
	newSession.player_id = g_playerID++;
	newSession.player_x = 40;
	newSession.player_y = 10;

	//새로 접속한 클라에게 남들 위치를 알려주기 
	std::list<SESSION*>::iterator iter;
	for (iter = sessionList.begin(); iter != sessionList.end(); ++iter)
	{
		_MSG msg;
		msg.type = STAR_SET_TYPE;
		msg.id = (*iter)->player_id;
		msg.x = (*iter)->player_x;
		msg.y = (*iter)->player_y;
		sendPacket_unicast(&newSession, (char*)&msg);
	}

	//list에 삽입
	sessionList.push_back(&newSession);

	//모든 클라에게 새로 접속한 클라 생성 뿌리기 (나 포함)
	_MSG msg;
	msg.type = STAR_SET_TYPE;
	msg.id = newSession.player_id;
	msg.x = newSession.player_x;
	msg.y = newSession.player_y;
	sendPacket_unicast(NULL, (char*)&msg);
}

void network(SOCKET listenSocket)
{
	FD_SET ReadSet;
	FD_ZERO(&ReadSet);
	FD_SET(listenSocket, &ReadSet);

	//전체를 순회하면서 세션 세팅.
	std::list<SESSION*>::iterator iter;
	for (iter = sessionList.begin(); iter != sessionList.end(); ++iter)
	{
		FD_SET((*iter)->socket, &ReadSet);
	}

	int ret = select(0, &ReadSet, NULL, NULL, NULL); //timeval을 무한으로 하는 이유 -> 서버는 새로운 일이 없으면 아무것도 할 일이 없다.
	if (ret > 0)
	{
		//listenSocket이 반응하면 accept하기
		if (FD_ISSET(listenSocket, &ReadSet))
		{
			serverAccept(listenSocket);
		}

		//세션 리스트 돌면서 FD_ISSET으로 니가 반응한 애냐고 물어보고, 걔네 대상으로 recv 시도.
		for (iter = sessionList.begin(); iter != sessionList.end(); ++iter)
		{
			if (FD_ISSET((*iter)->socket, &ReadSet))
			{
				//recv 
			}
		}
	}

}

void monitoring()
{

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
	servAddr.sin_port = htons(SERVERPORT);
	
	// IP 주소 PORT번호 할당
	ret = bind(serverSocket, (SOCKADDR*)&servAddr, sizeof(servAddr));
	if (ret == SOCKET_ERROR)
		return -1;

	// 생성한 소켓을 서버 소켓으로 완성
	ret = listen(serverSocket, 5);
	if(ret == SOCKET_ERROR)
		return -1;


	while (true)
	{

		network(serverSocket);
		
		
		//network(serverSocket, (SOCKADDR*)&clientAddr, &clientAddrsize);


		//monitoring();


		//clientSocket = accept(serverSocket, (SOCKADDR*)&clientAddr, &clientAddrsize);	// 클라이언트 연결요청 수락
		//if (clientSocket == INVALID_SOCKET)
		//	return -1;

		// accept된 클라이언트 데이터를 전송
		//send(clientSocket, message, sizeof(message), 0);

	}
	
	
	closesocket(serverSocket);

	// 종료전 초기화한 소켓 라이브러리 해제.
	WSACleanup();
	return 0;
}

