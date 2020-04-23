// Project1.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//
#include "stdafx.h"
#include "Client.h"
#include "CRingBuffer.h"
#include "protocol.h"


#define MAX_LOADSTRING 100
#define GET_X_LPARAM(lp)  ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)  ((int)(short)HIWORD(lp))


// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

bool g_connect = false;
bool g_isSend = false;

static int x = 0;
static int y = 0;
static int oldX = 0;
static int oldY = 0;

SOCKET Socket;
CRingBuffer sendQueue;
CRingBuffer recvQueue;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR    lpCmdLine, _In_ int  nCmdShow)
{
	char serverIP[30];
	printf("ip 입력 : ");
	scanf("%s", serverIP);



	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
	{
		MessageBox(0, L"Error", L"", MB_OK);
		return 0;
	}

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 여기에 코드를 입력합니다.

	// 전역 문자열을 초기화합니다.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_CLIENT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);
	
	// 윈도우 생성
	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 600, 500, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
		return FALSE;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIENT));

	MSG msg;

	//소켓 생성
	Socket = socket(AF_INET, SOCK_STREAM, 0);
	
	//비동기 상태로 변경
	WSAAsyncSelect(Socket, hWnd, WM_NETWORK, FD_CONNECT | FD_WRITE | FD_READ | FD_CLOSE);

	SOCKADDR_IN serveraddr;

	ZeroMemory(&serveraddr, sizeof(serveraddr));

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, serverIP, &serveraddr.sin_addr);
	//addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	int ret = connect(Socket, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (ret == SOCKET_ERROR)
	{
		//진짜 에러
		int err = WSAGetLastError();
		if (err != WSAEWOULDBLOCK)
			return -1;
	}


	// 기본 메시지 루프
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else //게임 루프
		{
			if(g_connect)
			{

			}
		}
	}

	closesocket(Socket);
	return (int)msg.wParam;
}

void drawLine(HWND hWnd, st_DRAW_PACKET* packet)
{
	HDC hdc = GetDC(hWnd);

	MoveToEx(hdc, packet->iEndX, packet->iEndY, NULL);
	LineTo(hdc, packet->iStartX, packet->iStartY);
	
	ReleaseDC(hWnd, hdc);
}

void procRead(HWND hWnd)
{
	char recvBuffer[5000];
	int ret = recv(Socket, recvBuffer, 5000, 0);
	if (ret == SOCKET_ERROR)
		return;

	recvQueue.Enqueue(recvBuffer, ret);

	unsigned short Header;
	st_DRAW_PACKET Packet;

	while (true)
	{
		//헤더의 길이보다 작으면 아직 덜왔다 판단 후 나가기
		if (recvQueue.GetUseSize() <= 2) break;

		recvQueue.Peek((char*)&Header, 2);

		//패킷 하나가 완성이 안됐으면(헤더에 담긴 패킷 길이 + 헤더 크기) 할 게 없으니 나가기
		if (recvQueue.GetUseSize() < Header + 2) break;

		//헤더 지나서 패킷만 읽음
		recvQueue.MoveFront(2);
		recvQueue.Dequeue((char*)&Packet, Header);

		//drawLine 함수 호출
		drawLine(hWnd, &Packet);
	}
}

//FD_WRITE가 떴을 때. 이 함수는 sendQueue.GetUseSize() <=0 될 때 까지 while문 돌려야 한다. 아니면 우드블럭 뜰 때 까지.
void procWrite()
{
	g_isSend = true; //이건 FD_WRITE 호출되고, 이 함수 불리기 전에 해주기 

	//큐에 있는 것 보내주기 
	char sendBuffer[5000]; //이걸 거쳐서 send 하는데 여기선 peek로 확인 
	int ret = sendQueue.Peek(sendBuffer, 5000);

	ret = send(Socket, sendBuffer, ret, 0);

	if (ret == SOCKET_ERROR)
	{
		int err = WSAGetLastError();

		//우드블럭이 아니라면 
		if (err != WSAEWOULDBLOCK)
			return;
		//우드블럭이 맞다면 -> 아무것도 카피를 못한 것. FD_WRITE가 뜨기 전까지 우드블럭, 더 확인할 필요 없다. isSend=false로 바꾸고 그냥 나간다. 나중에 FD_WRITE에서 true로 바꿔줄거다. 
		else
		{
			g_isSend = false;
			return;
		}

	}

	//에러가 나지 않았으면 뭐라도 갔다는 것. sendQueue에서 removeFront(ret)로 copy에 성공한 ret만큼만 지워줌
	sendQueue.MoveFront(ret);
}

void sendPacket(char* buffer, int size)
{
	unsigned short Header = size;

	int ret;
	ret = sendQueue.Enqueue((char*)&Header, 2);	//헤더 큐에 넣기 
	ret = sendQueue.Enqueue(buffer, size);		//패킷 큐에 넣기 

	if (ret < size) // 송신버퍼 다 찰걸 고려해서 sendQueue 만들었는데, 이것도 다 차면 끊어내야 한다. 서버도 마찬가지.
		return ;

	if (g_isSend == false)
		return;

	procWrite(); //그럼 sendPacket에서는 이것만 호출해주고 나가면 된다.
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLIENT));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_CLIENT);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	static bool isDraw = false;

	switch (message)
	{
	case WM_NETWORK:
	{
		WORD event = WSAGETSELECTEVENT(lParam);
		WORD error = WSAGETSELECTERROR(lParam);

		if (error)
			return -1;

		switch (event)
		{
		case FD_CONNECT:
			g_connect = true;
			break;
		case FD_READ:
			procRead(hWnd);
			break;
		case FD_WRITE:
			procWrite();
			break;
		case FD_CLOSE:
			break;
		}
		break;
	}
	case WM_MOUSEMOVE:
		oldX = x;
		oldY = y;
		x = LOWORD(lParam);
		y = HIWORD(lParam);

		if (isDraw)
		{
			st_DRAW_PACKET packet;

			packet.iStartX = x;
			packet.iStartY = y;
			packet.iEndX = oldX;
			packet.iEndY = oldY;

			sendPacket((char*)&packet, sizeof(st_DRAW_PACKET));
		}
		break;

	case WM_LBUTTONUP:
		isDraw = false;
		break;

	case WM_LBUTTONDOWN:
		isDraw = true;
		break;
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