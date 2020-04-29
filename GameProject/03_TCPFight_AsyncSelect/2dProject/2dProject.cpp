// 2dProject.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//
#pragma once

#include "framework.h"
#include "2dProject.h"
#include "CScreenDib.h"
#include "CSpriteDib.h"
#include "CBaseObject.h"
#include "CPlayerObject.h"
#include "CRingBuffer.h"
#include "CNetwork.h"

#define MAX_LOADSTRING 100


// 전역 변수:
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

bool g_connect = false;							//서버에 접속 되었는지(논블록 소켓이라 연결이 안됐을수도)
CRingBuffer g_RecvBuffer;						//수신용 버퍼 
CRingBuffer g_SendBuffer;						//송신용 버퍼
bool g_SendPossible = false;					//없어도 wouldblock에 걸리지만, 애초에 보내지 말자는 의미에서
SOCKET Socket;

CNetwork *network = new CNetwork();

list<CPlayerObject*> g_objectList;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void KeyProcess();
void Update();
void InitialGame();
void Action();
void Draw();
void objectSort();
bool FrameSkip(int deltaTime);

bool PosYComp(const CBaseObject* lhs, const CBaseObject* rhs) {
	return lhs->GetCurY() < rhs->GetCurY();
}
CSpriteDib g_cSprite(90 , 0x00ffffff);
CScreenDib g_cScreenDib(640, 480, 32);
HWND hWnd;

CBaseObject* g_pPlayerObject;

std::list<CBaseObject*> objectList;

bool g_bActiveApp = true; //지금 활성화 됐는지 안됐는지

bool flag = false;
void Update()
{
	if (g_bActiveApp == true)
		KeyProcess();

	Action();

	static int cur = timeGetTime();
	static int old = timeGetTime();
	static int deltaTime = cur - old;
	
	static int skiptime = 0;

	cur = timeGetTime();

	if (flag)
	{
		old = cur - (deltaTime - (20 - (cur - skiptime)));
		flag = false;
		printf("deltaTime : %d \t old : %d \t cur : %d oldCur : %d\n", 
			cur - old, old, cur, skiptime);
	}

	deltaTime = cur - old;

	
	//여기서 시작시간과 끝 시간을 재는 이유 -> draw를 패스할지 말지 정한다
	//프레임스킵 안해도 되면 그리고, 스킵해야하면 건너뛰자.
	if (false == FrameSkip(deltaTime))
	{
		if (deltaTime < 20)
		{
			Sleep(20 - deltaTime);
		}
		//20ms를 초과 X -> Sleep 한 시간만큼 old에 더한다.
		//20ms를 초과 O -> 초과한 시간만큼 old에서 뺀다. (그냥 누적하다가 1프레임이 넘으면 스킵)
		
		old = cur - (deltaTime - 20);

		Draw();
	}
	else
	{
		flag = true;
		skiptime = cur;
	}

	//프레임스킵 여부, 델타타입 결과값, curr 차감시킨값. 

	//screen dib을 화면으로 플립
	//dib버퍼의 내용을 화면으로 출력
	g_cScreenDib.Flip(hWnd);

}

bool FrameSkip(int deltaTime)
{
	if (deltaTime >= 40)
		return true;

	return false;
}

void Action()
{
	list<CBaseObject*>::iterator iter;
	for (iter = objectList.begin(); iter != objectList.end(); ++iter)
	{
		(*iter)->Run();
	}
}

void Draw()
{
	//출력버퍼 포인터 및 정보를 얻음
	BYTE* bypDest = g_cScreenDib.GetDibBuffer();
	int iDestWidth = g_cScreenDib.GetWidth();
	int iDestHeight = g_cScreenDib.GetHeight();
	int iDestPitch = g_cScreenDib.GetPitch();
	objectSort();

	//1. 맵 화면 출력
	g_cSprite.DrawImage(0, 0, 0, bypDest, iDestWidth, iDestHeight, iDestPitch);

	static wchar_t szFrame[15];
	static int iFrame = 0;
	static DWORD dwTick = timeGetTime();

	//2. 캐릭터 출력
	list<CBaseObject*>::iterator iter;
	for (iter = objectList.begin(); iter != objectList.end(); ++iter) 
	{
		(*iter)->Render(bypDest, iDestWidth, iDestHeight, iDestPitch);
	}

	iFrame++;

	if (dwTick + 1000 < timeGetTime())
	{
		wsprintf(szFrame, L"RFrame : %d   ", iFrame);
		iFrame = 0;
		dwTick = timeGetTime();
	}

	HDC hDC = GetDC(hWnd);
	TextOut(hDC, 700, 500, szFrame, wcslen(szFrame));

	//버퍼 포인터에 그림을 그린다
	//스트라이프 출력부

	ReleaseDC(hWnd, hDC);
}

void KeyProcess()
{
	if (g_pPlayerObject == NULL)
		return;

	if (GetAsyncKeyState('Q'))
	{
		g_pPlayerObject->ActionInput(dfACTION_ATTACK1);
	}
	else if (GetAsyncKeyState('W'))
	{
		g_pPlayerObject->ActionInput(dfACTION_ATTACK2);
	}
	else if (GetAsyncKeyState('E'))
	{
		g_pPlayerObject->ActionInput(dfACTION_ATTACK3);
	}
	else if (GetAsyncKeyState(VK_LEFT) && GetAsyncKeyState(VK_UP))
	{
		g_pPlayerObject->ActionInput(dfACTION_MOVE_LU);
	}
	else if (GetAsyncKeyState(VK_LEFT) && GetAsyncKeyState(VK_DOWN))
	{
		g_pPlayerObject->ActionInput(dfACTION_MOVE_LD);
	}
	else if (GetAsyncKeyState(VK_LEFT))
	{
		g_pPlayerObject->ActionInput(dfACTION_MOVE_LL);
	}
	else if (GetAsyncKeyState(VK_RIGHT) && GetAsyncKeyState(VK_UP))
	{
		g_pPlayerObject->ActionInput(dfACTION_MOVE_RU);
	}
	else if (GetAsyncKeyState(VK_RIGHT) && GetAsyncKeyState(VK_DOWN))
	{
		g_pPlayerObject->ActionInput(dfACTION_MOVE_RD);
	}
	else if (GetAsyncKeyState(VK_RIGHT))
	{
		g_pPlayerObject->ActionInput(dfACTION_MOVE_RR);
	}
	else if (GetAsyncKeyState(VK_UP))
	{
		g_pPlayerObject->ActionInput(dfACTION_MOVE_UU);
	}
	else if (GetAsyncKeyState(VK_DOWN))
	{
		g_pPlayerObject->ActionInput(dfACTION_MOVE_DD);
	}
	else
		g_pPlayerObject->ActionInput(dfACTION_STAND);
}

void InitialGame()
{
	g_cSprite.LoadDibSprite(eMap, _T("Sprite_Data\\_Map.bmp"), 0, 0);

	g_cSprite.LoadDibSprite(ePLAYER_STAND_L01, _T("Sprite_Data\\Stand_L_01.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_STAND_L02, _T("Sprite_Data\\Stand_L_02.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_STAND_L03, _T("Sprite_Data\\Stand_L_03.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_STAND_L04, _T("Sprite_Data\\Stand_L_03.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_STAND_L05, _T("Sprite_Data\\Stand_L_02.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_STAND_L_MAX, _T("Sprite_Data\\Stand_L_01.bmp"), 71, 90);

	g_cSprite.LoadDibSprite(ePLAYER_STAND_R01, _T("Sprite_Data\\Stand_R_01.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_STAND_R02, _T("Sprite_Data\\Stand_R_02.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_STAND_R03, _T("Sprite_Data\\Stand_R_03.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_STAND_R04, _T("Sprite_Data\\Stand_R_03.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_STAND_R05, _T("Sprite_Data\\Stand_R_02.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_STAND_R_MAX, _T("Sprite_Data\\Stand_R_01.bmp"), 71, 90);

	g_cSprite.LoadDibSprite(ePLAYER_ATTACK1_L01, _T("Sprite_Data\\Attack1_L_01.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK1_L02, _T("Sprite_Data\\Attack1_L_02.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK1_L03, _T("Sprite_Data\\Attack1_L_03.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK1_L04, _T("Sprite_Data\\Attack1_L_04.bmp"), 71, 90);

	g_cSprite.LoadDibSprite(ePLAYER_ATTACK1_R01, _T("Sprite_Data\\Attack1_R_01.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK1_R02, _T("Sprite_Data\\Attack1_R_02.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK1_R03, _T("Sprite_Data\\Attack1_R_03.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK1_R04, _T("Sprite_Data\\Attack1_R_04.bmp"), 71, 90);

	g_cSprite.LoadDibSprite(ePLAYER_ATTACK2_L01, _T("Sprite_Data\\Attack2_L_01.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK2_L02, _T("Sprite_Data\\Attack2_L_02.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK2_L03, _T("Sprite_Data\\Attack2_L_03.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK2_L04, _T("Sprite_Data\\Attack2_L_04.bmp"), 71, 90);

	g_cSprite.LoadDibSprite(ePLAYER_ATTACK2_R01, _T("Sprite_Data\\Attack2_R_01.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK2_R02, _T("Sprite_Data\\Attack2_R_02.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK2_R03, _T("Sprite_Data\\Attack2_R_03.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK2_R04, _T("Sprite_Data\\Attack2_R_04.bmp"), 71, 90);

	g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_L01, _T("Sprite_Data\\Attack3_L_01.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_L02, _T("Sprite_Data\\Attack3_L_02.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_L03, _T("Sprite_Data\\Attack3_L_03.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_L04, _T("Sprite_Data\\Attack3_L_04.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_L05, _T("Sprite_Data\\Attack3_L_05.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_L06, _T("Sprite_Data\\Attack3_L_06.bmp"), 71, 90);

	g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_R01, _T("Sprite_Data\\Attack3_R_01.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_R02, _T("Sprite_Data\\Attack3_R_02.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_R03, _T("Sprite_Data\\Attack3_R_03.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_R04, _T("Sprite_Data\\Attack3_R_04.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_R05, _T("Sprite_Data\\Attack3_R_05.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_ATTACK3_R06, _T("Sprite_Data\\Attack3_R_06.bmp"), 71, 90);

	g_cSprite.LoadDibSprite(ePLAYER_MOVE_L01, _T("Sprite_Data\\Move_L_01.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_L02, _T("Sprite_Data\\Move_L_02.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_L03, _T("Sprite_Data\\Move_L_03.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_L04, _T("Sprite_Data\\Move_L_04.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_L05, _T("Sprite_Data\\Move_L_05.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_L06, _T("Sprite_Data\\Move_L_06.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_L07, _T("Sprite_Data\\Move_L_07.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_L08, _T("Sprite_Data\\Move_L_08.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_L09, _T("Sprite_Data\\Move_L_09.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_L10, _T("Sprite_Data\\Move_L_10.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_L11, _T("Sprite_Data\\Move_L_11.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_L12, _T("Sprite_Data\\Move_L_12.bmp"), 71, 90);

	g_cSprite.LoadDibSprite(ePLAYER_MOVE_R01, _T("Sprite_Data\\Move_R_01.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_R02, _T("Sprite_Data\\Move_R_02.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_R03, _T("Sprite_Data\\Move_R_03.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_R04, _T("Sprite_Data\\Move_R_04.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_R05, _T("Sprite_Data\\Move_R_05.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_R06, _T("Sprite_Data\\Move_R_06.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_R07, _T("Sprite_Data\\Move_R_07.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_R08, _T("Sprite_Data\\Move_R_08.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_R09, _T("Sprite_Data\\Move_R_09.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_R10, _T("Sprite_Data\\Move_R_10.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_R11, _T("Sprite_Data\\Move_R_11.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_MOVE_R12, _T("Sprite_Data\\Move_R_12.bmp"), 71, 90);

	g_cSprite.LoadDibSprite(eEFFECT_SPARK_01, _T("Sprite_Data\\xSpark_1.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(eEFFECT_SPARK_02, _T("Sprite_Data\\xSpark_2.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(eEFFECT_SPARK_03, _T("Sprite_Data\\xSpark_3.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(eEFFECT_SPARK_04, _T("Sprite_Data\\xSpark_4.bmp"), 71, 90);

	g_cSprite.LoadDibSprite(eGUAGE_HP, _T("Sprite_Data\\HPGuage.bmp"), 0, 0);
	g_cSprite.LoadDibSprite(eSHADOW, _T("Sprite_Data\\Shadow.bmp"), 32, 4);


	//g_pPlayerObject = new CPlayerObject();
	//g_pPlayerObject->SetPosition(400, 100);
	//g_pPlayerObject->SetSprite(ePLAYER_STAND_R01, ePLAYER_STAND_R_MAX, dfDELAY_STAND);
	//g_pPlayerObject-> SetObjectID(0);
	//g_pPlayerObject->ActionInput(dfACTION_STAND);

	//objectList.push_back(g_pPlayerObject);
	//
	////더미 플레이어 생성 
	//CBaseObject* pObject;
	//for (int i = 0; i < 3; ++i)
	//{
	//	pObject = new CPlayerObject();
	//	pObject->SetPosition(150 + (i * 100), 150 + (i * 100));
	//	pObject->SetSprite(ePLAYER_STAND_R01, ePLAYER_STAND_R_MAX, dfDELAY_STAND);
	//	pObject->SetObjectID(1);
	//	pObject->ActionInput(dfACTION_STAND);

	//	objectList.push_back(pObject);
	//}

}

void objectSort()
{
	objectList.sort(PosYComp);
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.
	timeBeginPeriod(1);

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MY2DPROJECT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

	//1. 스프라이트 및 게임 데이터 로드
	InitialGame();

	//2. 아이피 입력을 위한 다이알로그 생성
	char serverIP[30];
	printf("ip 입력 : ");
	scanf("%s", serverIP);


    //3. 윈도우를 생성한다.
	hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 800, 700, nullptr, nullptr, hInstance, nullptr);
	if (!hWnd)
		return FALSE;
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);


	//4. 윈속 초기화 및 소켓 생성
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (Socket == INVALID_SOCKET)
		return 1;

	// connect()
	SOCKADDR_IN serveraddr;
	IN_ADDR Addr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));

	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET, serverIP, &serveraddr.sin_addr);
	serveraddr.sin_port = htons(SERVER_PORT);


	//5. WSAAsyncSelect 등록, 비동기 상태로 변경
	WSAAsyncSelect(Socket, hWnd, WM_NETWORK, FD_CONNECT | FD_WRITE | FD_READ | FD_CLOSE);
	
	//6. 서버로 connect 접속 시도
	int ret = connect(Socket, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (ret == SOCKET_ERROR)
	{
		//진짜 에러
		int err = WSAGetLastError();
		if (err != WSAEWOULDBLOCK)
			return -1;
	}

    MSG msg;

    // 기본 메시지 루프입니다:
	while (true)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if(g_connect)
				Update();
		}
	}


	objectList.clear();
	closesocket(Socket);
	timeEndPeriod(1);
    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY2DPROJECT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MY2DPROJECT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
		break;
	case WM_NETWORK:
	{
		WORD event = WSAGETSELECTEVENT(lParam);
		WORD error = WSAGETSELECTERROR(lParam);

		if (error)
			return -1;

		switch (event)
		{
		case FD_CONNECT:
			g_connect = true;
			return true;
		case FD_READ:
			if (!network->RecvEvent())
				return false;
			return true;
		case FD_WRITE:
			g_SendPossible = true;
			if (!network->SendEvent())
				return false;
			return true;
		case FD_CLOSE:
			return false;
		}
	}
		break;

	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_ACTIVE)
			g_bActiveApp = true;
		else if (LOWORD(wParam) == WA_CLICKACTIVE)
			g_bActiveApp = true;
		else if (LOWORD(wParam) == WA_INACTIVE)
			g_bActiveApp = false;
		break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
