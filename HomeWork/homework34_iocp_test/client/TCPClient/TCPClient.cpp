// TCPClient.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
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

// ���� �Լ� �������
void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&lpMsgBuf, 0, NULL);

	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);

}

// ����� ���� ������ �����Լ� (�Է��� �����ŭ ���������� ���)
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
	// ���� �ʱ�ȭ
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

	// ������ ��ſ� ����� ���� 
	WCHAR chBuf[BUFSIZE + 1];
	int iLen;

	// ������ ������ ���
	while (1)
	{
		// ������ �Է�
		wprintf(L"\n[���� ������] ");
		if (fgetws(chBuf, BUFSIZE + 1, stdin) == NULL)
			break;

		// ������ ������
		retval = send(sock, (char*)chBuf, (int)(wcslen(chBuf)*2 -2), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("send()");
			break;
		}

		wprintf(L"[TCP Ŭ���̾�Ʈ] %d ����Ʈ�� ���½��ϴ�. \n", retval);

		// ������ �ޱ�
		retval = recvn(sock, chBuf, retval, 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("recv()");
			break;
		}
		else if (retval == 0)
			break;

		// ���� ������ ���
		chBuf[retval] = '\0';
		wprintf(L"[TCP Ŭ���̾�Ʈ] %d ����Ʈ�� �޾ѽ��ϴ�.\n", retval);
		wprintf(L"[���� ������] %s\n", chBuf);
	}

	closesocket(sock);

	WSACleanup();
    return 0;
}

