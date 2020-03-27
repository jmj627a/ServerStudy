#include <Windows.h>
#include "CScreenDib.h"

//#pragma comment (lib, "winmm")

CScreenDib::CScreenDib(int iWidth, int iHeight, int iColorBit)
{
	memset(&m_stDibInfo, 0, sizeof(BITMAPINFO));

	m_bypBuffer = NULL;

	m_iWidth = 0;
	m_iHeight = 0;
	m_iPitch = 0;
	m_iColorBit = 0;
	m_iBufferSize = 0;

	//생성자의 인자로 버퍼를 받는다
	CreateDibBuffer(iWidth, iHeight, iColorBit);
}

CScreenDib::~CScreenDib()
{
	ReleaseDibBuffer();
}

void CScreenDib::CreateDibBuffer(int iWidth, int iHeight, int iColorBit)
{
	m_iWidth = iWidth;
	m_iHeight = iHeight;
	m_iColorBit = iColorBit;

	m_iPitch = ((m_iWidth * (m_iColorBit / 8)) + 3) & ~3; //올림해서 뒤에 2비트를 00으로 끝나게 만듦. 그냥 00으로 바꾸면 버림이니까
	m_iBufferSize = m_iPitch * m_iHeight;


	//DibInfo 헤더 생성
	//Dib 출력시 뒤집어서 출력하기 위해 높이값을 -로 입력하고 있다.
	m_stDibInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_stDibInfo.bmiHeader.biWidth = m_iWidth;
	m_stDibInfo.bmiHeader.biHeight = -m_iHeight;
	m_stDibInfo.bmiHeader.biPlanes = 1;
	m_stDibInfo.bmiHeader.biBitCount = m_iColorBit;
	m_stDibInfo.bmiHeader.biCompression= 0;
	m_stDibInfo.bmiHeader.biSizeImage= m_iBufferSize;
	m_stDibInfo.bmiHeader.biXPelsPerMeter= 0;
	m_stDibInfo.bmiHeader.biYPelsPerMeter= 0;
	m_stDibInfo.bmiHeader.biClrUsed = 0;
	m_stDibInfo.bmiHeader.biClrImportant= 0;

	//버퍼 생성
	m_bypBuffer = new BYTE[m_iBufferSize];
	memset(m_bypBuffer, 0xff, m_iBufferSize);
}

void CScreenDib::ReleaseDibBuffer(void)
{
	m_iWidth = 0;
	m_iHeight = 0;
	m_iPitch = 0;
	m_iBufferSize = 0;

	memset(&m_stDibInfo, 0x00, sizeof(BITMAPINFO));

	if (m_bypBuffer != NULL)
		delete[] m_bypBuffer;

	m_bypBuffer = NULL;
}

void CScreenDib::Flip(HWND hWnd, int iX, int iY)
{
	if(m_bypBuffer == NULL) return;

	RECT Rect;
	HDC hDC;


	//찍을 대상의 DC를 얻는다
	GetWindowRect(hWnd, &Rect);
	hDC = GetDC(hWnd);

	//비트맵을 원본 크기로 출력
	int i = SetDIBitsToDevice(hDC, 0, 0, m_iWidth, m_iHeight, 
		0, 0, 0, m_iHeight, m_bypBuffer, &m_stDibInfo, DIB_RGB_COLORS);


	//화면에 프레임 표시 부분, 필요에 따라 사용
	{
		static wchar_t szFrame[15];
		static int iFrame = 0;
		static DWORD dwTick = timeGetTime();

		iFrame++;

		if (dwTick + 1000 < timeGetTime())
		{
			wsprintf(szFrame, L"TFrame : %d   ", iFrame);
			iFrame = 0;
			dwTick = timeGetTime();
		}

		TextOut(hDC, 700, 470, szFrame, wcslen(szFrame));
		TextOut(hDC, 0, 0, szFrame, wcslen(szFrame));
	}
	ReleaseDC(hWnd, hDC);
}

BYTE* CScreenDib::GetDibBuffer(void)
{
	return m_bypBuffer;
}

int CScreenDib::GetWidth(void)
{
	return m_iWidth;
}

int CScreenDib::GetHeight(void)
{
	return m_iHeight;
}

int CScreenDib::GetPitch(void)
{
	return m_iPitch;
}
