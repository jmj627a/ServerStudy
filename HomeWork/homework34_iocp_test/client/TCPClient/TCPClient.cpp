// TCPClient.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <locale.h>
void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&lpMsgBuf, 0, NULL);

	MessageBoxA(NULL, (LPCSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류출력
void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&lpMsgBuf, 0, NULL);

	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);

}

// 사용자 정의 데이터 수신함수 (입력한 사이즈만큼 받을때까지 대기)
int recvn(SOCKET s, WCHAR *buf, int len, int flags)
{
	int received;
	WCHAR *ptr = buf;
	int left = len;

	while (left > 0)
	{
		received = recv(s, (char*)ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
	}
	return (len - left);
}

int main()
{
	_wsetlocale(LC_ALL, L"korean");
	int retval;
	ULONG IP;
	// 윈속 초기화
	WSADATA WSA;
	if (WSAStartup(MAKEWORD(2, 2), &WSA) != 0)
		return 1;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
		err_quit("socket()");

	// connect()
	InetPtonA(AF_INET, (SERVERIP), &IP);
	SOCKADDR_IN ServerAddr;
	ZeroMemory(&ServerAddr, sizeof(ServerAddr));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.s_addr = IP;
	ServerAddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR *)&ServerAddr, sizeof(ServerAddr));
	if (retval == SOCKET_ERROR)
		err_quit("connect()");

	// 데이터 통신에 사용할 변수 
	WCHAR chBuf[BUFSIZE + 1];
	int iLen;

	// 서버와 데이터 통신
	while (1)
	{
		// 데이터 입력
		wprintf(L"\n[보낼 데이터] ");
		if (fgetws(chBuf, BUFSIZE + 1, stdin) == NULL)
			break;

		// 데이터 보내기
		retval = send(sock, (char*)chBuf, (int)(wcslen(chBuf)*2 -2), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("send()");
			break;
		}

		wprintf(L"[TCP 클라이언트] %d 바이트를 보냈습니다. \n", retval);

		// 데이터 받기
		retval = recvn(sock, chBuf, retval, 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("recv()");
			break;
		}
		else if (retval == 0)
			break;

		// 받은 데이터 출력
		chBuf[retval] = '\0';
		wprintf(L"[TCP 클라이언트] %d 바이트를 받앗습니다.\n", retval);
		wprintf(L"[받은 데이터] %s\n", chBuf);
	}

	closesocket(sock);

	WSACleanup();
    return 0;
}

