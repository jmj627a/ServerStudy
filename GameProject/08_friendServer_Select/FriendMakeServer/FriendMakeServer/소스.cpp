#include "Protocol.h"
#include "Network.h"

CNetwork network;

int main()
{
	network.networkInit();
	network.networkFunc();

	//closesocket(g_listenSocket);

	// 종료전 초기화한 소켓 라이브러리 해제.
	WSACleanup();
	return 0;
}