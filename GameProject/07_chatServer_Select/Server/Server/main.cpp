#pragma once
#include <stdio.h>
#include "Network.h"



int main()
{
	networkInit();
	networkFunc();

	//closesocket(g_listenSocket);

	// ������ �ʱ�ȭ�� ���� ���̺귯�� ����.
	WSACleanup();
	return 0;
}

