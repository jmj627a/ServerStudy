#include "Protocol.h"
#include "Network.h"

CNetwork network;

int main()
{
	network.networkInit();
	network.networkFunc();

	//closesocket(g_listenSocket);

	// ������ �ʱ�ȭ�� ���� ���̺귯�� ����.
	WSACleanup();
	return 0;
}