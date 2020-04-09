#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#define SERVERPORT 3000
#define BUFSIZE    1600

struct _MSG
{
	int type;
	int id;
	int x;
	int y;
};


struct _PLAYER
{
	int connect;
	int id = -1;
	int x = -1;
	int y = -1;
};


int myID = -1;
bool isChangePos = false;
_PLAYER player[64];

bool myConnect(SOCKET socket, SOCKADDR_IN &serveraddr, int namelen)
{
	int ret = 0;
	ret = connect(socket, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	ret = GetLastError();

	FD_SET ReadSet;
	FD_ZERO(&ReadSet);
	FD_SET(socket, &ReadSet);

	timeval time;
	time.tv_usec = 500000;

	ret = select(0, &ReadSet, NULL, NULL, &time);
	if (ret > 0)
	{
		return true;
	}
	else
		return false;
}

bool keyProcess()
{
	isChangePos = false;

	//�� id�� ���� �� �� ��Ȳ
	if (myID == -1) 
		return false;

	//�� ��ǥ�� ���� -1�ΰ� -> �� ĳ���Ͱ� ���� ������ �� �� ��Ȳ
	if (player[myID].x == -1)
		return false;

	//�� id�� ��������, ĳ���͵� ������
	if (GetAsyncKeyState(VK_UP))
	{
		if (player[myID].y > 0)
		{
			player[myID].y--;
			isChangePos = true;
		}
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		if (player[myID].y < 22)
		{
			player[myID].y++;
			isChangePos = true;
		}
	}
	if (GetAsyncKeyState(VK_LEFT))
	{
		if (player[myID].x > 0)
		{
			player[myID].x--;
			isChangePos = true;
		}
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		if (player[myID].x < 79)
		{
			player[myID].x++;
			isChangePos = true;
		}
	}


	if (isChangePos)
		return true;
	else
		return false;
}

char buf[BUFSIZE];
bool network(SOCKET clientSock)
{
	FD_SET ReadSet;
	FD_ZERO(&ReadSet);
	FD_SET(clientSock, &ReadSet);

	timeval time;
	time.tv_usec = 500000;

	int ret = 100;
	ret = select(0, &ReadSet, NULL, NULL, &time);
	if (ret > 0)
	{
		int recvSize = recv(clientSock, buf, sizeof(buf), 0);	//select ����� ���� �� ������ �ޱ�
		if (recvSize == SOCKET_ERROR)
			return false;

		//���ۿ��� ������ 16����Ʈ�� ������ ���� �� ����������� ó��.
		_MSG* msg;
		char* ptr;
		ptr = buf;
		int count = 0;
		_PLAYER* tempPlayer;
		while (true)
		{
			//msg���ٰ� �������� type �з��ϱ�.
			msg = (_MSG*)ptr;

			switch (msg->type)
			{
			case 0:	//id �Ҵ�
				tempPlayer = (_PLAYER*)msg;
				myID = tempPlayer->id;
				break;

			case 1:	//�� ����
				tempPlayer = (_PLAYER*)msg;
				player[tempPlayer->id].id = tempPlayer->id;
				player[tempPlayer->id].x = tempPlayer->x;
				player[tempPlayer->id].y = tempPlayer->y;
				break;

			case 2:	//�� ����
				tempPlayer = (_PLAYER*)msg;
				player[tempPlayer->id].id = -1;
				player[tempPlayer->id].x = -1;
				player[tempPlayer->id].y = -1;
				break;

			case 3: //�̵�
				tempPlayer = (_PLAYER*)msg;
				player[tempPlayer->id].x = tempPlayer->x;
				player[tempPlayer->id].y = tempPlayer->y;
				break;
			}

			count += sizeof(_MSG);
			ptr += sizeof(_MSG);
			
			//recv�� ���� ��ŭ �����͸� �� �о��ų�, ���� �������� 1600�� �Ѿ��ٸ� �׸��б�
			if (count == recvSize || count > BUFSIZE)	
			{
				memset(buf, 0, sizeof(buf));
				break;
			}
		}
	}


	if (isChangePos)
	{
		//select�� �˻����� �ʾ����� ��� ������ ������� �翬�� ���������Ŷ� �����ϰ� ������
		_MSG sendMSG;
		sendMSG.type = 3;
		sendMSG.id = myID;
		sendMSG.x = player[myID].x;
		sendMSG.y = player[myID].y;

		send(clientSock, (const char*)(&sendMSG), sizeof(_MSG), 0);
	}

}

char screenBuffer[23][81];

void rendering()
{
	for (int i = 0; i < 23; ++i)
	{
		for (int j = 0; j < 81; ++j)
		{
			if(j == 80)
				screenBuffer[i][j] = '\n';
			else
				screenBuffer[i][j] = '.';
		}
	}


	for (int i = 0; i < 64; ++i)
	{
		if (player[i].id == -1)
			continue;

		screenBuffer[player[i].y][player[i].x] = '*';
	}

	printf("%s", screenBuffer);

}

int main(int argc, char* argv[])
{
	int retval;

	char serverIP[30];
	printf("ip �Է� : ");
	scanf("%s", serverIP);

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
		return 1;

	// connect()
	SOCKADDR_IN serveraddr;
	IN_ADDR Addr;

	ZeroMemory(&serveraddr, sizeof(serveraddr));

	serveraddr.sin_family = AF_INET;
	//inet_ntop(AF_INET, &(Addr.S_un.S_addr), serverIP, INET_ADDRSTRLEN);
	inet_pton(AF_INET, serverIP, &serveraddr.sin_addr);
	serveraddr.sin_port = htons(SERVERPORT);

	while (true)
	{
		if (!myConnect(sock, serveraddr, sizeof(serveraddr)))
			continue;

		while (true)
		{
			keyProcess();
			network(sock);
			system("cls");
			rendering();
		}

	}

	closesocket(sock);

	// ���� ����
	WSACleanup();
	return 0;
}










BOOL DomainToIP(const WCHAR* szDomain, IN_ADDR* pAddr)
{

	ADDRINFOW* pAddrInfo;
	SOCKADDR_IN* pSockAddr;

	int ret = GetAddrInfoW(szDomain, L"0", NULL, &pAddrInfo);
	if (ret != 0)
		return false;

	pSockAddr = (SOCKADDR_IN*)pAddrInfo->ai_addr;
	*pAddr = pSockAddr->sin_addr;

	/*for (ADDRINFOW* ai = pAddrInfo; ai != NULL; ai = ai->ai_next) {
		pSockAddr = (SOCKADDR_IN*)ai->ai_addr;
		inet_ntop(AF_INET, &(pSockAddr->sin_addr), buf, sizeof(buf));
		printf("IP address: %s\n", buf);
	}*/

	FreeAddrInfoW(pAddrInfo);
	return true;
}