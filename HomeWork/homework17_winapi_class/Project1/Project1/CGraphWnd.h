#pragma once
#include <Windows.h>
#include "global.h"
#include "CQueue.h"

class CGraphWnd
{
public:

	// �� Ŭ�������� �̱۶���,��Ƽ����,����ǥ��,���̱׷��� ��
	// �پ��� �׷��� ����� ��� �����ϰ� �ִ�.

	// ������ Ÿ�Ը��� �����Ͽ� �ش� �������� �׷����� ���� ���� ����
	// �� Ÿ���� ���� ���ν����� WM_PAINT ���� �б⸦ ����
	// �ش� Ÿ�Կ� �´� ���� �Լ��� ���� �� ����.

	enum TYPE
	{
		LINE_SINGLE,
		LINE_MULTI,
		NUMBER,
		ONOFF,
		PIE
	};


	// ������ ���ν����� ���� ������ �˰� �ǰ�����...
	// WndProc �� �Լ� �����Ͱ� ������ ������ ���� �Ǿ�� �Ѵ�.
	// Ŭ������ �ɹ� �Լ��� this �� �����ؾ� �ϹǷ� �ݹ� �Լ��� ����� �Ұ��� ��

	// �ᱹ WndProc �ɹ� �Լ��� static ���� ���� �Ǿ�� �Ѵ�.
	// ������ static �� WndProc ���� this �� �������� �ʱ� ������ �ɹ����� ������ �Ұ��� �ϴ�.

	// �̸� ���ؼ� �ᱹ ������ ������ Ư�� �������� �������� ���� this �� ã�Ƽ� Ȱ���ϴ� ������� ����.
	// �׷��� ������ �ڵ��� hwnd ���� �ش� �����츦 ������� ��ü�� �����͸� ���̺�� ��Ī���� this �� ã�Ƴ��߸� �Ѵ�.

	// HWND �� �ش� �����츦 ���� ��ü�� �����͸� ���ν�Ű�� ����ü.
	// �� ����ü�� ���� static �迭�� �����ϸ�, WndProc ������ �Ź� �˻��Ͽ� ����Ѵ�.

	// �̴� �������� user data �� extra data Ȱ������ �� ������ �ذᵵ ����������
	// �ϴ� api �� ���� ������ �ʴ� ��� ������ �ذ������� �غ����� �Ѵ�.

	typedef struct ST_HWNDtoTHIS
	{
		HWND		hWnd;
		CGraphWnd	*pThis;

	} stHWNDtoTHIS;

public:

	CGraphWnd(HINSTANCE hInstance, HWND hWndParent, TYPE enType, int iPosX, int iPosY, int iWidth, int iHeight);
	~CGraphWnd();

	/////////////////////////////////////////////////////////
	// ������ ���ν���
	/////////////////////////////////////////////////////////
	static LRESULT CALLBACK WndChildProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
			CQueue* queue = new CQueue();

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
				GetClientRect(hWnd, &Rect);		//�׸� �׷��� Ŭ���̾�Ʈ ������ ũ�⸦ ����. -> ������ ũ�� != Ŭ���̾�Ʈ ũ��

				HDC hdc = GetDC(hWnd);
				g_hMemDC = CreateCompatibleDC(hdc);		// createDC�� �� DC�����̶�, �װ� ���� �� �Լ��� �ʿ��ϴ�.  
				g_hMemBitmap = CreateCompatibleBitmap(hdc, Rect.right, Rect.bottom); // ���� ������� �Ȱ��� �Ӽ�, �������� HBITMAP ����
				SelectObject(g_hMemDC, g_hMemBitmap);		//DC�� bitmap�� ����
				ReleaseDC(hWnd, hdc);

				//Ư�� �������� dc�� ä���ִ� �Լ���, �Ʒ� �� dc�� ������� ä����.
				PatBlt(g_hMemDC, 0, 0, Rect.right, Rect.bottom, WHITENESS);		// bit�� �����ϴ� �Լ��� dc�� ���� ����ϴ� ������ �Լ�.

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
	/////////////////////////////////////////////////////////
	// ������ �ֱ�.
	/////////////////////////////////////////////////////////
	BOOL	InsertData(int iData);


protected:

	//------------------------------------------------------
	// ������ �ڵ�, this ������ ��Ī ���̺� ����.
	//------------------------------------------------------
	BOOL			PutThis(void);


	static CGraphWnd	*GetThis(HWND hWnd);
	static stHWNDtoTHIS 	HWNDTable[dfMAXCHILD];
	static int changeWindowPosition(int _y);

private:

	//------------------------------------------------------
	// �θ� ������ �ڵ�, �� ������ �ڵ�, �ν��Ͻ� �ڵ�
	//------------------------------------------------------
	//------------------------------------------------------
	// ������ ��ġ,ũ��,����, �׷��� Ÿ�� ��.. �ڷ�
	//------------------------------------------------------
	TYPE		_enGraphType;

	//------------------------------------------------------
	// ���� ���۸��� �޸� DC, �޸� ��Ʈ��
	//------------------------------------------------------
	HDC		_hMemDC;
	HBITMAP		_hBitmap;


	//------------------------------------------------------
	// ������
	//------------------------------------------------------
	//������ ť
	static CQueue* queue;
};