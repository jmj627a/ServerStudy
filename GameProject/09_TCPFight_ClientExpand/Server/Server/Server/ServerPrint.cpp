
#include "ServerPrint.h"

extern bool g_Shutdown;

//��� ���� ����� �α� ����
int g_iLogLevel = 0;

//�α� ����� �ʿ��� �ӽ� ����
WCHAR g_szLogBuff[1024] = { 0, };

void Log(const WCHAR *szString, int iLogLevel)
{
	wprintf(L"%s \n", szString);
}

void ServerControl(void)
{
	//Ű���� �⺻ ��Ʈ��, ���, Ǯ�� ����
	static bool bControlMode = false;

	// L : ��Ʈ�� lock
	// U : ��Ʈ�� unlock
	// Q : ���� ����

	if (_kbhit())
	{
		WCHAR ControlKey = _getwch();

		//Ű���� ���� ���
		if (L'u' == ControlKey || L'U' == ControlKey)
		{
			bControlMode = true;

			wprintf(L"Control Mode : Press Q - quit \n");
			wprintf(L"Control Mode : Press L - key lock\n");
		}

		//Ű���� ���� ���
		if ((L'l' == ControlKey || L'L' == ControlKey) && bControlMode)
		{
			bControlMode = false;

			wprintf(L"Control Lock ! : Press U - unlock \n");
		}

		//Ű���� ���� Ǯ�� ���¿��� Ư�� ���
		if ((L'q' == ControlKey || L'Q' == ControlKey) && bControlMode)
		{
			g_Shutdown = true;
		}
	}
}
