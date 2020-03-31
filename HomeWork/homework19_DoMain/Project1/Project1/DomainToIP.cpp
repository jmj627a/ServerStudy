#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

BOOL DomainToIP(WCHAR *szDomain, IN_ADDR *pAddr)
{

	ADDRINFOW *pAddrInfo;
	SOCKADDR_IN *pSockAddr;
	CHAR buf[100];
	int ret = GetAddrInfoW(szDomain, L"0", NULL, &pAddrInfo);
	if (ret != 0)
		return false;

	pSockAddr = (SOCKADDR_IN*)pAddrInfo->ai_addr;
	*pAddr = pSockAddr->sin_addr;

	for (ADDRINFOW *ai = pAddrInfo; ai != NULL; ai = ai->ai_next) {
		pSockAddr = (SOCKADDR_IN*)ai->ai_addr;
		inet_ntop(AF_INET, &(pSockAddr->sin_addr), buf, sizeof(buf));
		printf("IP address: %s\n", buf);
	}

	FreeAddrInfoW(pAddrInfo);
	return true;
}

int main()
{
	WSADATA    wsaData;
	// 윈속 초기화
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
	{
		printf("WSAStartup Error No : %d", WSAGetLastError());
		return 1;
	}

	//hostent *host;
	SOCKADDR_IN SockAddr;
	IN_ADDR Addr;
	memset(&SockAddr, 0, sizeof(SockAddr));


	WCHAR str[100];
	wprintf(L"Input Domain : ");
	wscanf(L"%s", str);
	wprintf(L"\n%s\n", str);

	DomainToIP(str, &Addr);

	// 윈속 종료
	WSACleanup();
	system("pause");
}