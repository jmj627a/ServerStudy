#include "protocol.h"
#include "ServerPrint.h"
#include "Network.h"
#include "Content.h"

bool g_Shutdown = false;

//출력 저장 대상의 로그 레벨
extern int g_iLogLevel;

//로그 저장시 필요한 임시 버퍼
extern WCHAR g_szLogBuff[1024];

int main()
{
	//타이머 해상도 높이기
	timeBeginPeriod(1);

	//게임 데이터, DB 데이터 로딩 등 
	//netInit();

	//네트워크 최화, 리슨소켓 생성, Listen
	netStartUp();

	DWORD timeTickCount;
	DWORD dwTick;

	timeTickCount = timeGetTime();
	dwTick = timeTickCount;


	//서버 메인 루프, 전역의 g_Shutdown 값에 의해 종료 결정
	while (!g_Shutdown)
	{
		//네트워크 송수신 처리
		netIOProcess();

		//업데이트는 게임의 로직 (월드, 캐릭터, 몬스터, 이벤트)
		//실제 게임에서 돌아가야 하는 로직을 처리한다

		//게임 로직 업데이트
		dwTick = timeGetTime();
		if (dwTick - timeTickCount >= 40)
		{
			Update();
			timeTickCount = dwTick - (timeTickCount - 40);
		}

		//키보드 입력을 통해서 서버를 제어할 경우 사용
		ServerControl();


		//모니터링 정보를 표시, 저장, 전송하는 경우 사용
		//ServerMornitor();
	}


	//서버의 종료 대기
	//DB에 저장할 데이터나, 다른 마무리 작업 끝난 뒤에 꺼야 한다

	netCleanUp();
	return 0;
}




