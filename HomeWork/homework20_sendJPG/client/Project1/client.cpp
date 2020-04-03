#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#define SERVERDOMAIN   L"procademyserver.iptime.org"
#define SERVERPORT 10010
#define BUFSIZE    512

struct st_PACKET_HEADER
{
	DWORD dwPacketCode; // 0x11223344 �츮�� ��ŶȮ�� ������

	WCHAR szName[32]; // �����̸�, �����ڵ� NULL ���� ��
	WCHAR szFileName[128]; // �����̸�, �����ڵ� NULL ���� ��
	int iFileSize;
};

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


int main(int argc, char* argv[])
{
	int retval;

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

	DomainToIP(SERVERDOMAIN, &Addr);

	ZeroMemory(&serveraddr, sizeof(serveraddr));
	
	serveraddr.sin_family = AF_INET;
	char ip[100];
	inet_ntop(AF_INET, &(Addr.S_un.S_addr), ip, INET_ADDRSTRLEN);
	inet_pton(AF_INET, ip, &serveraddr.sin_addr);
	serveraddr.sin_port = htons(SERVERPORT);
	
	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		return 1;

	// ������ ��ſ� ����� ����
	char buf[BUFSIZE + 1];
	//int len;



	FILE* pImageFile = fopen("����ī����.bmp", "rb");

	char* pImageBuffer;

	fseek(pImageFile, 0, SEEK_END);
	int imageSize = ftell(pImageFile);
	fseek(pImageFile, 0, SEEK_SET);

	pImageBuffer = (char*)malloc(imageSize);
	//free(pImageBuffer);
	fread(pImageBuffer, 1, imageSize, pImageFile);
	fclose(pImageFile);


	st_PACKET_HEADER packetHeader;

	packetHeader.dwPacketCode = 0x11223344;
	packetHeader.iFileSize = imageSize;
	lstrcpyW(packetHeader.szFileName, L"����ī����.jpg");
	lstrcpyW(packetHeader.szName, L"������");



	// ������ ������ ���
	
	// ������ ������
	retval = send(sock, (char*)&packetHeader, sizeof(st_PACKET_HEADER), 0);
	if (retval == SOCKET_ERROR) 
		return 1;
		
	printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\n", retval);

	retval = send(sock, pImageBuffer, imageSize, 0);
	if (retval == SOCKET_ERROR)
		return 1;

	printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\n", retval);


	// ������ �ޱ�
	retval = recv(sock, buf, sizeof(buf), 0);
	if (retval == SOCKET_ERROR) 
		return 1;

	// ���� ������ ���
	buf[retval] = '\0';
	printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� �޾ҽ��ϴ�.\n", retval);
	printf("[���� ������] %s\n", buf);
	

	// closesocket()
	closesocket(sock);

	// ���� ����
	WSACleanup();
	return 0;
}