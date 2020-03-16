// 2dProject.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "2dProject.h"
#include "CScreenDib.h"
#include "CSpriteDib.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

CSpriteDib g_cSprite(7 , 0x00ffffff);
CScreenDib g_cScreenDib(640, 480, 32);
HWND hWnd;

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
	g_cSprite.DrawImage(1, 100, 100, bypDest, iDestWidth, iDestHeight, iDestPitch);
	
	//버퍼 포인터에 그림을 그린다
	//스트라이프 출력부
	/*BYTE byGrayColor = 0;
	for (int iCount = 0; iCount < 480; iCount++)
	{
		memset(bypDest, byGrayColor, 640 * 4);
		bypDest += iDestPitch;
		byGrayColor++;
	}*/

	//screen dib을 화면으로 플립
	//dib버퍼의 내용을 화면으로 출력
	g_cScreenDib.Flip(hWnd);
}

BOOL GameInit()
{
	//g_cSprite.LoadDibSprite(0, (const wchar_t*)("Sprite_Data\\_Map.bmp"), 0, 0);
	g_cSprite.LoadDibSprite(0, _T("Sprite_Data\\_Map.bmp"), 0, 0);
	g_cSprite.LoadDibSprite(1, _T("Sprite_Data\\Stand_L_01.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(2, _T("Sprite_Data\\Stand_L_02.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(3, _T("Sprite_Data\\Stand_L_03.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(4, _T("Sprite_Data\\Stand_R_01.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(5, _T("Sprite_Data\\Stand_R_02.bmp"), 71, 90);
	g_cSprite.LoadDibSprite(6, _T("Sprite_Data\\Stand_R_03.bmp"), 71, 90);

	return true;
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
	GameInit();


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
