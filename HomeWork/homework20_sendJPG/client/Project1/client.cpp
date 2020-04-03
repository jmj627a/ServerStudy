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
	DWORD dwPacketCode; // 0x11223344 우리의 패킷확인 고정값

	WCHAR szName[32]; // 본인이름, 유니코드 NULL 문자 끝
	WCHAR szFileName[128]; // 파일이름, 유니코드 NULL 문자 끝
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

	// 윈속 초기화
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

	// 데이터 통신에 사용할 변수
	char buf[BUFSIZE + 1];
	//int len;



	FILE* pImageFile = fopen("프로카데미.bmp", "rb");

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
	lstrcpyW(packetHeader.szFileName, L"프로카데미.jpg");
	lstrcpyW(packetHeader.szName, L"정민지");



	// 서버와 데이터 통신
	
	// 데이터 보내기
	retval = send(sock, (char*)&packetHeader, sizeof(st_PACKET_HEADER), 0);
	if (retval == SOCKET_ERROR) 
		return 1;
		
	printf("[TCP 클라이언트] %d바이트를 보냈습니다.\n", retval);

	retval = send(sock, pImageBuffer, imageSize, 0);
	if (retval == SOCKET_ERROR)
		return 1;

	printf("[TCP 클라이언트] %d바이트를 보냈습니다.\n", retval);


	// 데이터 받기
	retval = recv(sock, buf, sizeof(buf), 0);
	if (retval == SOCKET_ERROR) 
		return 1;

	// 받은 데이터 출력
	buf[retval] = '\0';
	printf("[TCP 클라이언트] %d바이트를 받았습니다.\n", retval);
	printf("[받은 데이터] %s\n", buf);
	

	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}