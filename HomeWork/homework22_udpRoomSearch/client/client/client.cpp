#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE    512


int main(int argc, char* argv[])
{
	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET)
		return -1;


	//��ε�ĳ���� Ȱ��ȭ 
	int nTimeOutValue = 200;
	BOOL enable = true;
	int ret = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&nTimeOutValue, sizeof(nTimeOutValue));
	ret = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (const char*)&enable, sizeof(enable));
	if (ret == SOCKET_ERROR)
		return -1;

	// ���� �ּ� ����ü �ʱ�ȭ
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr("255.255.255.255");
	
	//serveraddr.sin_port = htons(SERVERPORT);

	// ������ ��ſ� ����� ����
	SOCKADDR_IN peeraddr;
	int addrlen;
	char buf[BUFSIZE + 1];
	int len;

	char protocol[10];
	protocol[0] = 0xff;
	protocol[1] = 0xee;
	protocol[2] = 0xdd;
	protocol[3] = 0xaa;
	protocol[4] = 0x00;
	protocol[5] = 0x99;
	protocol[6] = 0x77;
	protocol[7] = 0x55;
	protocol[8] = 0x33;
	protocol[9] = 0x11;
	// ������ ������ ���
	for (int i = 0; i < 99; i++)
	{
		// ������ ������
		serveraddr.sin_port = htons(10001 + i);
		retval = sendto(sock, protocol, sizeof(protocol), 0, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
		retval = WSAGetLastError();
		if (retval == SOCKET_ERROR) {
			continue;
		}
		printf(".");

		// ������ �ޱ�
		addrlen = sizeof(peeraddr);
		retval = recvfrom(sock, buf, BUFSIZE, 0, (SOCKADDR*)&peeraddr, &addrlen);
		//if (retval == SOCKET_ERROR) {
		//	continue;
		//}

		if (retval == -1)
			continue;

		// ���� ������ ���
		printf("\n[UDP Ŭ���̾�Ʈ] ip : %s \n", inet_ntoa(peeraddr.sin_addr));
		printf("[UDP Ŭ���̾�Ʈ] port : %d \n", ntohs(peeraddr.sin_port));
		printf("[UDP Ŭ���̾�Ʈ] RoomName:");
		char* ptr = buf;
		for(int j=0; j<retval;++j)
			printf("%s", ptr++);

		printf("\n");
	}

	// closesocket()
	closesocket(sock);

	// ���� ����
	WSACleanup();
	return 0;
}