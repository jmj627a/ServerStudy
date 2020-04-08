#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#define SERVERDOMAIN   L"procademyserver.iptime.org"
#define SERVERPORT 3000
#define BUFSIZE    1600

struct MSG
{
	int type;
	int id;
	int x;
	int y;
};


struct PLAYER
{
	int id = -1;
	int x = -1;
	int y = -1;
};


int myID = -1;
bool isChangePos = false;
PLAYER player[64];

bool myConnect(SOCKET socket, const sockaddr* serveraddr, int namelen)
{
	FD_SET ReadSet;
	FD_ZERO(&ReadSet);

	int ret = 0;
	ret = select(0, &ReadSet, NULL, NULL, 0);
	if (ret > 0)
	{
		ret = connect(socket, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
		if (ret == SOCKET_ERROR)
			return 1;
		return true;
	}

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
		player[myID].x--;
		isChangePos = true;
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		player[myID].x++;
		isChangePos = true;
	}
	if (GetAsyncKeyState(VK_LEFT))
	{
		player[myID].y--;
		isChangePos = true;
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		player[myID].y++;
		isChangePos = true;
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

	int ret = 100;

	ret = select(0, &ReadSet, NULL, NULL, 0);
	if (ret > 0)
	{
		ret = recv(clientSock, buf, sizeof(buf), 0);	//select ����� ���� �� ������ �ޱ�
		if (ret == SOCKET_ERROR)
			return false;

		//���ۿ��� ������ 16����Ʈ�� ������ ���� �� ����������� ó��.
		PLAYER tempPlayer;
		char* ptr = buf;
		int count = 0;
		while (true)
		{
			//�ƾƤ��� �÷��̾ ������ �ȵǰ� ��Ŷ�� �޾ƾ��ϴ±��� msg���ٰ� �ް� �ϱ�.



















			memcpy(&tempPlayer, ptr, sizeof(PLAYER));

			player[tempPlayer.id].x = tempPlayer.x;
			player[tempPlayer.id].y = tempPlayer.y;

			count += sizeof(PLAYER);
			ptr += sizeof(PLAYER);
			if (ptr == NULL || count > BUFSIZE)	//�̰� �ٵ� 1600���� �޸� �ôµ� �ξƴϸ� �ȸ����ٵ�
			{
				memset(buf, 0, sizeof(buf));
				break;
			}
		}

	}


	if (isChangePos)
	{
		//select�� �˻����� �ʾ����� ��� ������ ������� �翬�� ���������Ŷ� �����ϰ� ������
		send(clientSock, (const char*)(&player[myID]), sizeof(PLAYER), 0);
	}

}


char screenBuffer[23][81];

bool rendering()
{
	for (int i = 0; i < 23; ++i)
	{
		for (int j = 0; j < 81; ++j)
		{
			if(j == 80)
				screenBuffer[i][j] = '\n';
			else
				screenBuffer[i][j] = ' ';
		}
	}


	for (int i = 0; i < 64; ++i)
	{
		if (player[i].id == -1)
			continue;

		screenBuffer[player[i].x][player[i].y] = '*';
	}

	for (int i = 0; i < 23; ++i)
		printf("%s \n", screenBuffer[i]);
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
	inet_ntop(AF_INET, &(Addr.S_un.S_addr), serverIP, INET_ADDRSTRLEN);
	inet_pton(AF_INET, serverIP, &serveraddr.sin_addr);
	serveraddr.sin_port = htons(SERVERPORT);

	while (true)
	{
		myConnect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));


		while (true)
		{

			keyProcess();
			
			network(sock);

			rendering();

		}

	}

	// closesocket()
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