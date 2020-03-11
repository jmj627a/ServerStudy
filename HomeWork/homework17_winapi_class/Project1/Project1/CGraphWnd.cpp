#include <Windows.h>
#include <cstdlib>
#include <time.h>

#include "Resource.h"
#include "CGraphWnd.h"
#include "CQueue.h"

CQueue* CGraphWnd:: queue = new CQueue();
CGraphWnd:: stHWNDtoTHIS HWNDTable[dfMAXCHILD];

CGraphWnd::CGraphWnd(HINSTANCE hInstance, HWND hWndParent, TYPE enType, int iPosX, int iPosY, int iWidth, int iHeight)
{
	initChildClass(hInstance);
	_enGraphType = enType;

	PutThis();

	CreateWindowW(L"Child", NULL,
		WS_CHILD | WS_VISIBLE /*| WS_BORDER*/ | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_DLGFRAME | WS_THICKFRAME | WS_TABSTOP,
		rand()%500, rand() % 500, CHILD_WINDOW_WIDTH, CHILD_WINDOW_HEIGHT,
		hWndParent, NULL, hInstance, NULL);
}

CGraphWnd::~CGraphWnd()
{
}


HDC	g_hMemDC1;
HBITMAP	g_hMemBitmap1;
HDC hdc1;
RECT Rect1;

LRESULT CGraphWnd::WndChildProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int oldX = 0;
	static int oldY = 0;
	static bool isDraw = false;

	switch (message)
	{
	case WM_CREATE:
	{
		pThis->createFunc(hWnd);

	}
	break;
	case WM_TIMER:
	{
		switch (wParam) {
		case LINE_SINGLE:

			if (queue->count >= 100)
			{
				NODE* temp = new NODE();
				bool ret = queue->deq(temp);
			}

			queue->enq(changeWindowPosition(rand() % CHILD_WINDOW_HEIGHT / 2));
			break;

		case LINE_MULTI:

			if (queue->count >= 100)
			{
				NODE* temp = new NODE();
				bool ret = queue->deq(temp);
			}

			queue->enq(changeWindowPosition(rand() % CHILD_WINDOW_HEIGHT / 2));
			break;
		}
		InvalidateRect(hWnd, NULL, false);
		break;
	}
	break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;

		PatBlt(g_hMemDC1, 0, 0, Rect1.right, Rect1.bottom, WHITENESS);

		oldX = 0;
		oldY = changeWindowPosition(0);

		for (int i = 0; i <= queue->count; ++i)
		{
			if (queue->getIndex(i) == -10)
				break;

			MoveToEx(g_hMemDC1, oldX, oldY, NULL);
			LineTo(g_hMemDC1, i * 10, queue->getIndex(i));

			oldX = i * 10;
			oldY = queue->getIndex(i);
		}

		hdc1 = BeginPaint(hWnd, &ps);
		BitBlt(hdc1, 0, 0, Rect1.right, Rect1.bottom, g_hMemDC1, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
	{
		DeleteObject(g_hMemBitmap1);
		DeleteObject(g_hMemDC1);
		KillTimer(hWnd, 2);
		PostQuitMessage(0);
	}
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

BOOL CGraphWnd::InsertData(int iData)
{
	return 0;
}

ATOM CGraphWnd::initChildClass(HINSTANCE hInstance)
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

BOOL CGraphWnd::PutThis(void)
{
	return 0;
}

CGraphWnd* CGraphWnd::GetThis(HWND hWnd)
{
	return nullptr;
}

int CGraphWnd::changeWindowPosition(int _y)
{
	return CHILD_WINDOW_HEIGHT - _y;
}

void CGraphWnd::createFunc(HWND hWnd)
{
	GetClientRect(hWnd, &Rect1);		//�׸� �׷��� Ŭ���̾�Ʈ ������ ũ�⸦ ����. -> ������ ũ�� != Ŭ���̾�Ʈ ũ��

	HDC hdc = GetDC(hWnd);
	g_hMemDC1 = CreateCompatibleDC(hdc);		// createDC�� �� DC�����̶�, �װ� ���� �� �Լ��� �ʿ��ϴ�.  
	g_hMemBitmap1 = CreateCompatibleBitmap(hdc, Rect1.right, Rect1.bottom); // ���� ������� �Ȱ��� �Ӽ�, �������� HBITMAP ����
	SelectObject(g_hMemDC1, g_hMemBitmap1);		//DC�� bitmap�� ����
	ReleaseDC(hWnd, hdc);

	//Ư�� �������� dc�� ä���ִ� �Լ���, �Ʒ� �� dc�� ������� ä����.
	PatBlt(g_hMemDC1, 0, 0, Rect1.right, Rect1.bottom, WHITENESS);		// bit�� �����ϴ� �Լ��� dc�� ���� ����ϴ� ������ �Լ�.

	srand((unsigned)time(NULL));

	SetTimer(hWnd, _enGraphType, 100, NULL);
}
