// project.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "pch.h"
#include "framework.h"
#include "project.h"
#include "RBTree.h"

#define MAX_LOADSTRING 100


// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

HWND g_hWnd;
HDC g_hMemDC;
HBITMAP g_hMemBitmap;
HBITMAP g_hMemBitmapold;
// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
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
    LoadStringW(hInstance, IDC_PROJECT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, WIN_WIDTH, WIN_HEIGHT, nullptr, nullptr, hInstance, nullptr);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PROJECT));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PROJECT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_PROJECT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}
 
RECT	Rect;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static RBTree* tree = new RBTree();
    static int nextData = rand() % 1000;
    static int flag = 0; //0-nothing 1-insert 2-delete
    static TCHAR inputStr[256] = { };
    static RECT rect;
    switch (message)
    {
    case WM_LBUTTONDOWN:
        tree->InsertNode(nextData);
        nextData = rand() % 1000;
        GetClientRect(hWnd, &rect);
        InvalidateRect(hWnd, &rect, true);
        break;


    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_F1:
            flag = 1;
            break;

        case VK_F2:
            tree->InsertNode(nextData);
            nextData = rand() % 1000;
            GetClientRect(hWnd, &rect);
            InvalidateRect(hWnd, &rect, true);
            break;
        case VK_F3:
            flag = 2;
            break;
        case VK_RETURN:
            if (flag == 1)
            {
                tree->InsertNode(_wtoi(inputStr));
                flag = 0;
                ZeroMemory(inputStr, 256);
            }
            else if (flag == 2)
            {
                tree->deleteNode(_wtoi(inputStr));
                flag = 0;
                ZeroMemory(inputStr, 256);
            }
            GetClientRect(hWnd, &rect);
            InvalidateRect(hWnd, &rect, true);
            break;
        }
        break;
    case WM_CHAR:
    {
        TCHAR data[100];
        HDC hdc = GetDC(hWnd);

        int len;
        
        if (flag != 0)
        {
            len = lstrlen(inputStr);
            inputStr[len] = (TCHAR)wParam;
            inputStr[len + 1] = 0;
        }
        
        if (flag == 1)
        {
            wsprintf(data, TEXT("삽입데이터 :%s "), inputStr);
            TextOut(hdc, 100, 50, data, wcslen(data));
        }
        else if (flag == 2)
        {
            wsprintf(data, TEXT("삭제데이터 :%s "), inputStr);
            TextOut(hdc, 100, 50, data, wcslen(data));
        }
        ReleaseDC(hWnd, hdc);
    }
    break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = GetDC(hWnd);

            tree->draw_Node(hdc, tree->m_pRoot,1,WIN_WIDTH/2, 0);

            SetTextColor(hdc, RGB(0, 0, 0));
            TCHAR text[100];
            wsprintf(text, TEXT("f1(삽입-데이터입력), f2(삽입-랜덤숫자) , f3(삭제-데이터입력)"));
            TextOut(hdc, 100, 30, text, wcslen(text));

            if (flag == 1)
            {
                wsprintf(text, TEXT("삽입데이터 :"));
                TextOut(hdc, 100, 50, text, wcslen(text));
            }
            else if (flag == 2)
            {
                wsprintf(text, TEXT("삭제데이터 :"));
                TextOut(hdc, 100, 50, text, wcslen(text));
            }
            else
            {
                wsprintf(text, TEXT("                             "));
                TextOut(hdc, 100, 50, text, wcslen(text));
            }

            TCHAR data[20];
            wsprintf(data, TEXT("next : %d      "), nextData);
            TextOut(hdc, 100, 10, data, wcslen(data));

            ReleaseDC(hWnd, hdc);
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
 