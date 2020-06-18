
#include "ServerPrint.h"

extern bool g_Shutdown;

//출력 저장 대상의 로그 레벨
int g_iLogLevel = 0;

//로그 저장시 필요한 임시 버퍼
WCHAR g_szLogBuff[1024] = { 0, };

void Log(const WCHAR *szString, int iLogLevel)
{
	wprintf(L"%s \n", szString);
}

void ServerControl(void)
{
	//키보드 기본 컨트롤, 잠금, 풀림 변수
	static bool bControlMode = false;

	// L : 컨트롤 lock
	// U : 컨트롤 unlock
	// Q : 서버 종료

	if (_kbhit())
	{
		WCHAR ControlKey = _getwch();

		//키보드 제어 허용
		if (L'u' == ControlKey || L'U' == ControlKey)
		{
			bControlMode = true;

			wprintf(L"Control Mode : Press Q - quit \n");
			wprintf(L"Control Mode : Press L - key lock\n");
		}

		//키보드 제어 잠금
		if ((L'l' == ControlKey || L'L' == ControlKey) && bControlMode)
		{
			bControlMode = false;

			wprintf(L"Control Lock ! : Press U - unlock \n");
		}

		//키보드 제어 풀림 상태에서 특정 기능
		if ((L'q' == ControlKey || L'Q' == ControlKey) && bControlMode)
		{
			g_Shutdown = true;
		}
	}
}
