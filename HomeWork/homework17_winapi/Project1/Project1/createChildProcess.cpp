// Project1.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//http://egloos.zum.com/EireneHue/v/966008
//http://blog.naver.com/PostView.nhn?blogId=tipsware&logNo=221699922371&categoryNo=0&parentCategoryNo=0&viewDate=&currentPage=4&postListTopCurrentPage=1&from=postList

#include "framework.h"
#include "Project1.h"
#include "CQueue.h"


#define MAX_LOADSTRING 100
#define GET_X_LPARAM(lp)  ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)  ((int)(short)HIWORD(lp))
#define WINDOW_WIDTH		1400
#define WINDOW_HEIGHT		1000

#define CHILD_WINDOW_WIDTH		500
#define CHILD_WINDOW_HEIGHT		300

int changeWindowPosition(int _y)
{
	return CHILD_WINDOW_HEIGHT - _y;
}


// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
ATOM                RegisterChildClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    WndChildProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

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
	RegisterChildClass(hInstance);

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

ATOM RegisterChildClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndChildProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PROJECT1));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_PROJECT1);
	wcex.lpszClassName = L"Child";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

HWND hWnd;

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

	hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
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


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int mouseOldX, mouseX = 0;
	static int mouseOldY, mouseY = 0;
	static bool isDraw = false;
	static CQueue* queue = new CQueue();
	HWND childhWnd1;
	HWND childhWnd2;

	HPEN pen, oldPen;

	switch (message)
	{
	case WM_CREATE:
	{
		childhWnd1 = CreateWindowW(L"Child", NULL,
			WS_CHILD | WS_VISIBLE /*| WS_BORDER*/ | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_DLGFRAME | WS_THICKFRAME | WS_TABSTOP,
			10, 10, CHILD_WINDOW_WIDTH, CHILD_WINDOW_HEIGHT,
			hWnd, NULL, hInst, NULL);

		childhWnd2 = CreateWindowW(L"Child", NULL,
			WS_CHILD | WS_VISIBLE /*| WS_BORDER*/ | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_DLGFRAME | WS_THICKFRAME | WS_TABSTOP,
			CHILD_WINDOW_WIDTH + 10, 10, CHILD_WINDOW_WIDTH, CHILD_WINDOW_HEIGHT,
			hWnd, NULL, hInst, NULL);
	}
	break;
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
		HDC hdc = BeginPaint(hWnd, &ps);

		if (isDraw)
		{
			pen = CreatePen(PS_SOLID, rand() % 10 + 1, RGB(rand() % 255, rand() % 255, rand() % 255)); // (RGB(rand() % 255, rand() % 255, rand() % 255));
			oldPen = (HPEN)SelectObject(hdc, pen);

			MoveToEx(hdc, mouseOldX, mouseOldY, NULL);
			LineTo(hdc, mouseX, mouseY);

			SelectObject(hdc, oldPen);
			DeleteObject(pen);
		}

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
	{
		PostQuitMessage(0);
	}
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK WndChildProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int oldX = 0;
	static int oldY = 0;
	static bool isDraw = false;
	static CQueue* queue = new CQueue();

	switch (message)
	{
	case WM_CREATE:
	{
		SetTimer(hWnd, 1, 100, NULL);
	}
	break;
	case WM_TIMER:
	{
		switch (wParam) {
		case 1:

			if (queue->count >= 100)
			{
				NODE* temp = new NODE();
				bool ret = queue->deq(temp);
			}

			queue->enq(changeWindowPosition(rand() % CHILD_WINDOW_HEIGHT / 3));
			break;
		}
		InvalidateRect(hWnd, NULL, true);
		break;
	}
	break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		oldX = 0;
		oldY = changeWindowPosition(0);

		for (int i = 0; i <= queue->count; ++i)
		{
			if (queue->getIndex(i) == -10)
				break;

			MoveToEx(hdc, oldX, oldY, NULL);
			LineTo(hdc, i * 10, queue->getIndex(i));

			oldX = i * 10;
			oldY = queue->getIndex(i);
		}


		//TextOut(hdc, oldX, oldY, L"Beautiful Korea", wcslen(L"Beautiful Korea"));
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
	{
		KillTimer(hWnd, 1);
		PostQuitMessage(0);
	}
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}



// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

