// DIB.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "DIB.h"
#include <stdio.h>


int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	MSG msg;
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DIB));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_DIB);
	wcex.lpszClassName = L"myWindow";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	RegisterClassEx(&wcex);

	HWND hWnd = CreateWindow(L"myWindow", L"myWindow", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
	if (!hWnd) return FALSE;

	ShowWindow(hWnd, nCmdShow);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		BitmapDraw(hdc);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void BitmapDraw(HDC hDC)
{
	HANDLE	hFile;
	DWORD	dwRead;
	int		iPitch;
	int		iImageSize;
	BYTE	*bypImage;

	BITMAPFILEHEADER	stFileHeader;
	BITMAPINFOHEADER	stInfoHeader; 
	WCHAR	szFileName[] = L"procademy.bmp";

	FILE* pFile = NULL;
	_wfopen_s(&pFile, szFileName, L"rb");
	if (pFile == NULL) return;

	dwRead = fread(&stFileHeader, sizeof(stFileHeader), 1, pFile);

	if ( dwRead != 1 )
		MessageBox(NULL, L"파일 헤더읽기 에러", L"dd", MB_OK);
	dwRead = fread(&stInfoHeader, sizeof(stInfoHeader), 1, pFile);
	if ( dwRead != 1 )
		MessageBox(NULL, L"인포 헤더읽기 에러", L"dd", MB_OK);

	iPitch = ((stInfoHeader.biWidth * (stInfoHeader.biBitCount / 8)) + 3) & ~3;
	iImageSize = iPitch * stInfoHeader.biHeight;

	bypImage = (BYTE *)malloc(iImageSize);
	fread(bypImage, iImageSize, 1, pFile);
	fclose(pFile);

	StretchDIBits(hDC, 0, 0, stInfoHeader.biWidth, stInfoHeader.biHeight, 0, 0, 
		stInfoHeader.biWidth, stInfoHeader.biHeight, bypImage, (BITMAPINFO *)&stInfoHeader, DIB_RGB_COLORS, SRCCOPY);

	free(bypImage);
}