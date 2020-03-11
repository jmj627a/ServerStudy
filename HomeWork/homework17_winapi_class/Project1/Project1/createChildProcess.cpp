// Project1.cpp : ���ø����̼ǿ� ���� �������� �����մϴ�.
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


// ���� ����:
HINSTANCE hInst;                                // ���� �ν��Ͻ��Դϴ�.
WCHAR szTitle[MAX_LOADSTRING];                  // ���� ǥ���� �ؽ�Ʈ�Դϴ�.
WCHAR szWindowClass[MAX_LOADSTRING];            // �⺻ â Ŭ���� �̸��Դϴ�.

// �� �ڵ� ��⿡ ���Ե� �Լ��� ������ �����մϴ�:
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

	// TODO: ���⿡ �ڵ带 �Է��մϴ�.

	// ���� ���ڿ��� �ʱ�ȭ�մϴ�.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_PROJECT1, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);
	//p1->RegisterChildClass(hInstance);


	// ���ø����̼� �ʱ�ȭ�� �����մϴ�:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PROJECT1));

	MSG msg;

	// �⺻ �޽��� �����Դϴ�:
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
//  �Լ�: MyRegisterClass()
//
//  �뵵: â Ŭ������ ����մϴ�.
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
	hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.

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
		GetClientRect(hWnd, &Rect);		//�׸� �׷��� Ŭ���̾�Ʈ ������ ũ�⸦ ����. -> ������ ũ�� != Ŭ���̾�Ʈ ũ��

		HDC hdc = GetDC(hWnd);
		g_hMemDC = CreateCompatibleDC(hdc);		// createDC�� �� DC�����̶�, �װ� ���� �� �Լ��� �ʿ��ϴ�.  
		g_hMemBitmap = CreateCompatibleBitmap(hdc, Rect.right, Rect.bottom); // ���� ������� �Ȱ��� �Ӽ�, �������� HBITMAP ����
		SelectObject(g_hMemDC, g_hMemBitmap);		//DC�� bitmap�� ����
		ReleaseDC(hWnd, hdc);

		//Ư�� �������� dc�� ä���ִ� �Լ���, �Ʒ� �� dc�� ������� ä����.
		PatBlt(g_hMemDC, 0, 0, Rect.right, Rect.bottom, BLACKNESS);		// bit�� �����ϴ� �Լ��� dc�� ���� ����ϴ� ������ �Լ�.


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
		// �׸��⸦ �����Ѵ�. 
	case WM_LBUTTONUP:
		isDraw = false;
		return 0;
	case WM_LBUTTONDOWN:
		isDraw = true;
		return 0;
		// �迭�� ������ �о� ȭ���� �����Ѵ�. 

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
