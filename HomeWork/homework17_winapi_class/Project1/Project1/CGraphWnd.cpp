#include "CGraphWnd.h"
#include "CQueue.h"

CQueue* CGraphWnd:: queue = new CQueue();

CGraphWnd::CGraphWnd(HINSTANCE hInstance, HWND hWndParent, TYPE enType, int iPosX, int iPosY, int iWidth, int iHeight)
{
}

CGraphWnd::~CGraphWnd()
{
}

LRESULT CGraphWnd::WndChildProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC	g_hMemDC;
	HBITMAP	g_hMemBitmap;

	static int oldX = 0;
	static int oldY = 0;
	static bool isDraw = false;
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
		InvalidateRect(hWnd, NULL, false);
		break;
	}
	break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;

		RECT Rect;
		GetClientRect(hWnd, &Rect);		//그림 그려질 클라이언트 영역의 크기를 얻음. -> 윈도우 크기 != 클라이언트 크기

		HDC hdc = GetDC(hWnd);
		g_hMemDC = CreateCompatibleDC(hdc);		// createDC는 쌩 DC생성이라, 그거 말고 이 함수가 필요하다.  
		g_hMemBitmap = CreateCompatibleBitmap(hdc, Rect.right, Rect.bottom); // 현재 윈도우와 똑같은 속성, 색상으로 HBITMAP 생성
		SelectObject(g_hMemDC, g_hMemBitmap);		//DC와 bitmap을 연결
		ReleaseDC(hWnd, hdc);

		//특정 패턴으로 dc를 채워주는 함수로, 아래 는 dc를 흰색으로 채워줌.
		PatBlt(g_hMemDC, 0, 0, Rect.right, Rect.bottom, WHITENESS);		// bit로 시작하는 함수는 dc에 뭔가 출력하는 종류의 함수.

		oldX = 0;
		oldY = changeWindowPosition(0);

		for (int i = 0; i <= queue->count; ++i)
		{
			if (queue->getIndex(i) == -10)
				break;

			MoveToEx(g_hMemDC, oldX, oldY, NULL);
			LineTo(g_hMemDC, i * 10, queue->getIndex(i));

			oldX = i * 10;
			oldY = queue->getIndex(i);
		}

		hdc = BeginPaint(hWnd, &ps);
		BitBlt(hdc, 0, 0, Rect.right, Rect.bottom, g_hMemDC, 0, 0, SRCCOPY);
		DeleteObject(g_hMemBitmap);
		DeleteObject(g_hMemDC);
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

BOOL CGraphWnd::InsertData(int iData)
{
	return 0;
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