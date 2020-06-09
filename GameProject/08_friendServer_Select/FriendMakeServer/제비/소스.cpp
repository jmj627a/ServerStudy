#include "Protocol.h"
#include "Network.h"

CNetwork network;

int main()
{
	timeBeginPeriod(1);
	DWORD timeTickCount;
	DWORD dwTick;

	network.networkInit();

	timeTickCount = GetTickCount64();
	dwTick = timeTickCount;

	while (true)
	{
		if (GetAsyncKeyState(VK_SPACE))
		{
			network.testFunc_flagFalse();
		}

		network.networkFunc();

		dwTick = GetTickCount64();
		if (dwTick - timeTickCount > 1000)
		{
			network.printText();
			dwTick = 0;
			timeTickCount = GetTickCount64();
		}
	}
	//closesocket(g_listenSocket);

	// 종료전 초기화한 소켓 라이브러리 해제.
	WSACleanup();
	return 0;
}