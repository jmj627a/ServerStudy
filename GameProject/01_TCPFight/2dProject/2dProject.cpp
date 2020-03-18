// 2dProject.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include <list>
#include "framework.h"
#include "2dProject.h"
#include "CScreenDib.h"
#include "CSpriteDib.h"
#include "CBaseObject.h"
#include "CPlayerObject.h"

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

CSpriteDib g_cSprite(81 , 0x00ffffff);
CScreenDib g_cScreenDib(640, 480, 32);
HWND hWnd;

CBaseObject* g_pPlayerObject;

std::list<CBaseObject*> objectList;

bool g_bActiveApp = true; //지금 활성화 됐는지 안됐는지

void Update()
{
	//출력버퍼 포인터 및 정보를 얻음
	BYTE *bypDest = g_cScreenDib.GetDibBuffer();
	int iDestWidth = g_cScreenDib.GetWidth();
	int iDestHeight = g_cScreenDib.GetHeight();
	int iDestPitch= g_cScreenDib.GetPitch();


	//1. 맵 화면 출력
	g_cSprite.DrawImage(0, 0, 0, bypDest, iDestWidth, iDestHeight, iDestPitch);

	//2. 캐릭터 출력

	for (int i = 0; i < objectList.size; ++i)
	{
		//
	}
	
	//버퍼 포인터에 그림을 그린다
	//스트라이프 출력부


	if (g_bActiveApp)
		KeyProcess();

	//Action;
	//Draw();


	//screen dib을 화면으로 플립
	//dib버퍼의 내용을 화면으로 출력
	g_cScreenDib.Flip(hWnd);

	Sleep(0);
}

//void Update_Game(void)
//{
//	if (g_bActiveApp)
//		KeyProcess();
//
//	//Action;
//	//Draw();
//
//	g_cScreenDib.Flip(hWnd);
//	Sleep(0);
//}

void KeyProcess()
{
	if (g_pPlayerObject == NULL)
		return;

	//if ((GetAsyncKeyState(VK_UP) && GetAsyncKeyState(VK_LEFT)))
	//{
	//	g_pPlayerObject->SetSprite()
	//}
}

void InitialGame()
{
	//g_cSprite.LoadDibSprite(0, (const wchar_t*)("Sprite_Data\\_Map.bmp"), 0, 0);
	g_cSprite.LoadDibSprite(eMap, _T("Sprite_Data\\_Map.bmp"), 0, 0);

	g_cSprite.LoadDibSprite(ePLAYER_STAND_L01, _T("Sprite_Data\\Stand_L_01.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_STAND_L02, _T("Sprite_Data\\Stand_L_02.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_STAND_L03, _T("Sprite_Data\\Stand_L_03.bmp"), 71, 90);

	g_cSprite.LoadDibSprite(ePLAYER_STAND_R01, _T("Sprite_Data\\Stand_R_01.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_STAND_R02, _T("Sprite_Data\\Stand_R_02.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(ePLAYER_STAND_R03, _T("Sprite_Data\\Stand_R_03.bmp"), 71, 90);

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

	g_cSprite.LoadDibSprite(eGUAGE_HP, _T("Sprite_Data\\HPGuage.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(eSHADOW, _T("Sprite_Data\\Shadow.bmp"), 71, 90);


	g_pPlayerObject = new CPlayerObject();
	g_pPlayerObject->SetPosition(200, 100);
	g_pPlayerObject->SetSprite(ePLAYER_STAND_R01, ePLAYER_STAND_R_MAX, dfDELAY_STAND);

	objectList.push_back(g_pPlayerObject);
	
	//더미 플레이어 생성 
	CBaseObject* pObject;
	for (int i = 0; i < 3; ++i)
	{
		pObject = new CPlayerObject();
		pObject->SetPosition(200 + (i * 20), 100 + (i * 20));
		pObject->SetSprite(ePLAYER_STAND_R01, ePLAYER_STAND_R_MAX, dfDELAY_STAND);

		objectList.push_back(pObject);
	}

}





int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

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

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY2DPROJECT));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
			Update();
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
		else
		{
			//Update();
			//Update_Game();
		}
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
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
			g_bActiveApp = true;
		else
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
