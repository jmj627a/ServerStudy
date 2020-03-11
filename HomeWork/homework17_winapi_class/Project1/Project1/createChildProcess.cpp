// Project1.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//http://egloos.zum.com/EireneHue/v/966008
//http://blog.naver.com/PostView.nhn?blogId=tipsware&logNo=221699922371&categoryNo=0&parentCategoryNo=0&viewDate=&currentPage=4&postListTopCurrentPage=1&from=postList

#include "framework.h"
#include "Project1.h"
#include "CQueue.h"
#include "global.h"
#include "CGraphWnd.h"

#define MAX_LOADSTRING 100
#define GET_X_LPARAM(lp)  ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)  ((int)(short)HIWORD(lp))
#define WINDOW_WIDTH		1400
#define WINDOW_HEIGHT		1000


// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
//ATOM                RegisterChildClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    WndChildProc(HWND, UINT, WPARAM, LPARAM);


CGraphWnd *p1;
CGraphWnd *p2;

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
	LoadStringW(hInstance, IDC_PROJECT1, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);
	//p1->RegisterChildClass(hInstance);


	// 애플리케이션 초기화를 수행합니다:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PROJECT1));

	MSG msg;

	// 기본 메시지 루프입니다:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PROJECT1));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);//(HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_PROJECT1);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

HWND hWnd;

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

	hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT, 0, WINDOW_WIDTH, WINDOW_HEIGHT, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	//ShowWindow(childhWnd, nCmdShow);
	//UpdateWindow(childhWnd);

	return TRUE;
}


HDC	g_hMemDC;
HBITMAP	g_hMemBitmap;
HDC hdc;
RECT Rect;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	static int mouseOldX, mouseX = 0;
	static int mouseOldY, mouseY = 0;
	static bool isDraw = false;

	HWND childhWnd1;
	HWND childhWnd2;

	HPEN pen, oldPen;

	switch (message)
	{
	case WM_CREATE:
	{
		GetClientRect(hWnd, &Rect);		//그림 그려질 클라이언트 영역의 크기를 얻음. -> 윈도우 크기 != 클라이언트 크기

		HDC hdc = GetDC(hWnd);
		g_hMemDC = CreateCompatibleDC(hdc);		// createDC는 쌩 DC생성이라, 그거 말고 이 함수가 필요하다.  
		g_hMemBitmap = CreateCompatibleBitmap(hdc, Rect.right, Rect.bottom); // 현재 윈도우와 똑같은 속성, 색상으로 HBITMAP 생성
		SelectObject(g_hMemDC, g_hMemBitmap);		//DC와 bitmap을 연결
		ReleaseDC(hWnd, hdc);

		//특정 패턴으로 dc를 채워주는 함수로, 아래 는 dc를 흰색으로 채워줌.
		PatBlt(g_hMemDC, 0, 0, Rect.right, Rect.bottom, BLACKNESS);		// bit로 시작하는 함수는 dc에 뭔가 출력하는 종류의 함수.


		p1 = new CGraphWnd(hInst, hWnd, CGraphWnd::LINE_SINGLE, 10, 10, 200, 200);
		p2 = new CGraphWnd(hInst, hWnd, CGraphWnd::LINE_SINGLE, 220, 10, 200, 200);

		SetTimer(hWnd, 1, 100, NULL);

		//childhWnd1 = CreateWindowW(L"Child", NULL,
		//	WS_CHILD | WS_VISIBLE /*| WS_BORDER*/ | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_DLGFRAME | WS_THICKFRAME | WS_TABSTOP,
		//	10, 10, CHILD_WINDOW_WIDTH, CHILD_WINDOW_HEIGHT,
		//	hWnd, NULL, hInst, NULL);
		//
		//childhWnd2 = CreateWindowW(L"Child", NULL,
		//	WS_CHILD | WS_VISIBLE /*| WS_BORDER*/ | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_DLGFRAME | WS_THICKFRAME | WS_TABSTOP,
		//	CHILD_WINDOW_WIDTH + 10, 10, CHILD_WINDOW_WIDTH, CHILD_WINDOW_HEIGHT,
		//	hWnd, NULL, hInst, NULL);
	}
	break;

	case WM_TIMER:
		switch (wParam)
		{
		case 1:
			InvalidateRect(hWnd, NULL, false);
			//p1->InsertData(rand() % 100);
			//p2->InsertData(rand() % 100);
			break;
		}
		return 0;

	case WM_MOUSEMOVE:
		mouseOldX = mouseX;
		mouseOldY = mouseY;
		mouseX = LOWORD(lParam);
		mouseY = HIWORD(lParam);
		InvalidateRect(hWnd, NULL, false);
		return 0;
		// 그리기를 종료한다. 
	case WM_LBUTTONUP:
		isDraw = false;
		return 0;
	case WM_LBUTTONDOWN:
		isDraw = true;
		return 0;
		// 배열의 정보를 읽어 화면을 복구한다. 

	case WM_PAINT:
	{
		PAINTSTRUCT ps;

		if (isDraw)
		{
			pen = CreatePen(PS_SOLID, rand() % 10 + 1, RGB(rand() % 255, rand() % 255, rand() % 255)); // (RGB(rand() % 255, rand() % 255, rand() % 255));
			oldPen = (HPEN)SelectObject(g_hMemDC, pen);

			MoveToEx(g_hMemDC, mouseOldX, mouseOldY, NULL);
			LineTo(g_hMemDC, mouseX, mouseY);

			SelectObject(g_hMemDC, oldPen);
			DeleteObject(pen);
		}

		hdc = BeginPaint(hWnd, &ps);
		BitBlt(hdc, 0, 0, Rect.right, Rect.bottom, g_hMemDC, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
	{
		DeleteObject(g_hMemBitmap);
		DeleteObject(g_hMemDC);
		PostQuitMessage(0);
	}
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
