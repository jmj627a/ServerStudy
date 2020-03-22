// 2dProject.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include <list>
#include <Windows.h>

#include "framework.h"
#include "2dProject.h"
#include "CScreenDib.h"
#include "CSpriteDib.h"
#include "CBaseObject.h"
#include "CPlayerObject.h"

#pragma comment (lib, "winmm")

#define MAX_LOADSTRING 100


using namespace std;

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void KeyProcess();
//void Update_Game(void);
void Update();
void InitialGame();
void Action();
void Draw();
void objectSort();

bool PosYComp(const CBaseObject* lhs, const CBaseObject* rhs) {

	return lhs->GetCurY() < rhs->GetCurY();

}
CSpriteDib g_cSprite(90 , 0x00ffffff);
CScreenDib g_cScreenDib(640, 480, 32);
HWND hWnd;

CBaseObject* g_pPlayerObject;

std::list<CBaseObject*> objectList;
//CBaseObject* objectList[100];

bool g_bActiveApp = true; //지금 활성화 됐는지 안됐는지

void Update()
{
	if (g_bActiveApp == true)
		KeyProcess();


	Action();
	Draw();


	//screen dib을 화면으로 플립
	//dib버퍼의 내용을 화면으로 출력
	g_cScreenDib.Flip(hWnd);

	Sleep(0);
}

void Action()
{
	list<CBaseObject*>::iterator iter;
	for (iter = objectList.begin(); iter != objectList.end(); ++iter)
	{
		(*iter)->Run();
	}
	//for (int i = 0; i < 100; ++i)
	//{
	//	if (objectList[i] == NULL)
	//		continue;
	//	objectList[i]->Run();
	//}
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
	//for (int i = 0; i < 100; ++i)
	//{
	//	if (objectList[i] == NULL)
	//		continue;
	//
	//	objectList[i]->Render(bypDest, iDestWidth, iDestHeight, iDestPitch);
	//}

	iFrame++;

	if (dwTick + 1000 < timeGetTime())
	{
		wsprintf(szFrame, L"RFrame : %d   ", iFrame);
		iFrame = 0;
		dwTick = timeGetTime();
	}

	TextOut(GetDC(hWnd), 1000, 500, szFrame, wcslen(szFrame));

	//list<CBaseObject*>::iterator itr;
	//for (itr = objectList.begin(); itr != objectList.end(); ++itr)
	//{
	//	itr->;
	//}

	//버퍼 포인터에 그림을 그린다
	//스트라이프 출력부

}

void KeyProcess()
{
	if (g_pPlayerObject == NULL)
		return;


	if (GetAsyncKeyState(VK_LEFT))
		TextOut(GetDC(hWnd), 1000, 100, L"VK_LEFT ", 8);
	if (GetAsyncKeyState(VK_RIGHT))
		TextOut(GetDC(hWnd), 1000, 100, L"VK_RIGHT", 8);
	if (GetAsyncKeyState(VK_UP))
		TextOut(GetDC(hWnd), 1000, 100, L"VK_UP   ", 8);
	if (GetAsyncKeyState(VK_DOWN))
		TextOut(GetDC(hWnd), 1000, 100, L"VK_DOWN ", 8);

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
	//g_cSprite.LoadDibSprite(0, (const wchar_t*)("Sprite_Data\\_Map.bmp"), 0, 0);
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


	g_pPlayerObject = new CPlayerObject();
	g_pPlayerObject->SetPosition(400, 100);
	g_pPlayerObject->SetSprite(ePLAYER_STAND_R01, ePLAYER_STAND_R_MAX, dfDELAY_STAND);
	g_pPlayerObject-> SetObjectID(0);

	objectList.push_back(g_pPlayerObject);
	//for (int i = 0; i < 100; ++i)
	//{
	//	if (objectList[i] != NULL)
	//		continue;
	//
	//	objectList[i] = g_pPlayerObject;
	//	break;
	//}


	//더미 플레이어 생성 
	CBaseObject* pObject;
	for (int i = 0; i < 3; ++i)
	{
		pObject = new CPlayerObject();
		pObject->SetPosition(150 + (i * 100), 150 + (i * 100));
		pObject->SetSprite(ePLAYER_STAND_R01, ePLAYER_STAND_R_MAX, dfDELAY_STAND);
		pObject->SetObjectID(1);

		objectList.push_back(pObject);
		//for (int j = 0; j < 100; ++j)
		//{
		//	if (objectList[j] != NULL)
		//		continue;
		//
		//	objectList[j] = pObject;
		//	break;
		//}
	}

}

void objectSort()
{
	objectList.sort(PosYComp);
	//for (int i = 0; i < 100; ++i)
	//{
	//	if (objectList[i] == NULL) continue;
	//	for (int j = 0; j < 100; ++j)
	//	{
	//		if (objectList[j] == NULL) continue;
	//		if (objectList[i]->GetCurY() < objectList[j]->GetCurY())
	//		{
	//			CBaseObject* temp = objectList[i];
	//			objectList[i] = objectList[j];
	//			objectList[j] = temp;
	//		}
	//	}
	//}
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

	//이미지 불러오기
	InitialGame();


    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    //HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY2DPROJECT));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (true)
    {
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
			if (msg.message == WM_QUIT)
				break;
			//Update();
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
		else
			Update();
    }

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


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
		SetTimer(hWnd, 1, 100, NULL);
		break;
	case WM_TIMER:
		break;
	case WM_MOUSEMOVE:
		break;
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_ACTIVE)
			g_bActiveApp = true;
		else if (LOWORD(wParam) == WA_CLICKACTIVE)
			g_bActiveApp = true;
		else if(LOWORD(wParam) == WA_INACTIVE)
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
