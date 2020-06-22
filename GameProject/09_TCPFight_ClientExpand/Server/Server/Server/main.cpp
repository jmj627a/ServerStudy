#include "protocol.h"
#include "ServerPrint.h"
#include "Network.h"
#include "Content.h"

bool g_Shutdown = false;

//��� ���� ����� �α� ����
extern int g_iLogLevel;

//�α� ����� �ʿ��� �ӽ� ����
extern WCHAR g_szLogBuff[1024];

int main()
{
	//Ÿ�̸� �ػ� ���̱�
	timeBeginPeriod(1);

	//���� ������, DB ������ �ε� �� 
	//netInit();

	//��Ʈ��ũ ��ȭ, �������� ����, Listen
	netStartUp();

	DWORD timeTickCount;
	DWORD dwTick;

	timeTickCount = timeGetTime();
	dwTick = timeTickCount;


	//���� ���� ����, ������ g_Shutdown ���� ���� ���� ����
	while (!g_Shutdown)
	{
		//��Ʈ��ũ �ۼ��� ó��
		netIOProcess();

		//������Ʈ�� ������ ���� (����, ĳ����, ����, �̺�Ʈ)
		//���� ���ӿ��� ���ư��� �ϴ� ������ ó���Ѵ�

		//���� ���� ������Ʈ
		dwTick = timeGetTime();
		if (dwTick - timeTickCount >= 40)
		{
			Update();
			timeTickCount = dwTick - (timeTickCount - 40);
		}

		//Ű���� �Է��� ���ؼ� ������ ������ ��� ���
		ServerControl();


		//����͸� ������ ǥ��, ����, �����ϴ� ��� ���
		//ServerMornitor();
	}


	//������ ���� ���
	//DB�� ������ �����ͳ�, �ٸ� ������ �۾� ���� �ڿ� ���� �Ѵ�

	netCleanUp();
	return 0;
}




