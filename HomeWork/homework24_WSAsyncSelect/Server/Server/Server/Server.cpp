// Server.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "Server.h"

#define MAX_LOADSTRING 100


struct SESSION
{
	SOCKET Socket;
	SOCKADDR_IN AddrInfo;
	CRingBuffer recvQueue;
	CRingBuffer sendQueue;
	bool sendFlag;
};

SOCKET g_listenSocket;
list<SESSION*> sessionList;


// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
HWND hWnd;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
	{
		MessageBox(0, L"Error", L"", MB_OK);
		return 0;
	}

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SERVER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 윈도우 생성
	hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 600, 500, nullptr, nullptr, hInstance, nullptr);
	
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SERVER));


	//listenSocket 생성, bind, listen;
	g_listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (g_listenSocket == SOCKET_ERROR)
		return -1;

	SOCKADDR_IN servAddr;
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(SERVER_PORT);

	// IP 주소 PORT번호 할당
	int ret = bind(g_listenSocket, (SOCKADDR*)&servAddr, sizeof(servAddr));
	if (ret == SOCKET_ERROR)
		return -1;

	// 생성한 소켓을 서버 소켓으로 완성
	ret = listen(g_listenSocket, SOMAXCONN);
	if (ret == SOCKET_ERROR)
		return -1;

	//비동기 상태로 변경
	WSAAsyncSelect(g_listenSocket, hWnd, WM_NETWORK, FD_ACCEPT | FD_CLOSE);


	// 기본 메시지 루프
    MSG msg;
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	closesocket(g_listenSocket);
	return (int)msg.wParam;
}

SESSION *FindSession(SOCKET ClientSocket)
{
	//SessionList 에서 ClientSocket 검색 후 Session * 리턴
	list<SESSION*>::iterator iter;
	for (iter = sessionList.begin(); iter != sessionList.end(); iter++)
	{
		if (ClientSocket == (*iter)->Socket)
			return *iter;
	}

	//검색에 실패 했다면 return null;
	return NULL;
}

void RemoveSession(SOCKET p)
{
	int socknum = p;
	closesocket(p);

	//해당 세션 List 에서 삭제.
	std::list<SESSION*>::iterator iter;
	for (iter = sessionList.begin(); iter != sessionList.end(); iter)
	{
		if (p == (*iter)->Socket)
			iter = sessionList.erase(iter);
		else
			iter++;
	}
	

	//세션종료에 대한 로그 출력
	printf("[%d] 클라이언트 접속 종료. \n", socknum);
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SERVER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SERVER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

void ProcAccept()
{
	SOCKADDR_IN addr;
	int addrLen = sizeof(addr);
	SOCKET acceptSocket = accept(g_listenSocket, (SOCKADDR*)&addr, &addrLen); ;

	//Session 동적 할당.
	SESSION* newSession = new SESSION();
	newSession->Socket = acceptSocket;
	newSession->AddrInfo = addr;
	newSession->sendFlag = false;

	//SessionList 추가
	sessionList.push_back(newSession);

	WSAAsyncSelect(acceptSocket, hWnd, WM_NETWORK, FD_READ | FD_WRITE | FD_CLOSE);

	//접속자에 대한 로그 출력.
	printf("[%d] 클라이언트 접속. \n", newSession->Socket);
}

void ProcWrite(SESSION* session)
{
	session->sendFlag = true;
	int ret;
	while (1)
	{
		//sendQ에 있는거 바로 보내는것 해보기 send(session->Socket, session->SendQ 열심히 샌드
		//큐에 있는 것 보내주기 
		char sendBuffer[5000]; //이걸 거쳐서 send 하는데 여기선 peek로 확인 
		ret = session->sendQueue.Peek(sendBuffer, 5000);

		ret = send(session->Socket, sendBuffer, ret, 0);

		if (ret == SOCKET_ERROR)
		{
			int err = WSAGetLastError();

			//우드블럭이 아니라면 
			if (err != WSAEWOULDBLOCK)
			{
				RemoveSession(session->Socket);
				return;
			}//우드블럭이 맞다면 -> 아무것도 카피를 못한 것. FD_WRITE가 뜨기 전까지 우드블럭, 더 확인할 필요 없다. isSend=false로 바꾸고 그냥 나간다. 나중에 FD_WRITE에서 true로 바꿔줄거다. 
			else
			{
				session->sendFlag = false;
				return;
			}

		}

		session->sendQueue.MoveFront(ret);
		
		//session->SendQ 를 모두 보내면 반복문 중단
		if (session->sendQueue.GetUseSize() == 0)
			break;
	}
}

void SendPacket(SESSION* session, unsigned short *Header, st_DRAW_PACKET* Packet)
{
	session->sendQueue.Enqueue((char*)Header, 2);
	session->sendQueue.Enqueue((char*)Packet, 16);

	if (session->sendFlag == false)
		return;

	ProcWrite(session);
}

void SendBroadcast(unsigned short *Header, st_DRAW_PACKET* Packet)
{
	//SessionList 순회
	std::list<SESSION*>::iterator iter;
	for (iter = sessionList.begin(); iter != sessionList.end(); )
	{
		SESSION* p = *iter;
		iter++;
		
		SendPacket(p, Header, Packet);
	}
}

void drawLine(HWND hWnd, st_DRAW_PACKET* packet)
{
	HDC hdc = GetDC(hWnd);

	MoveToEx(hdc, packet->iEndX, packet->iEndY, NULL);
	LineTo(hdc, packet->iStartX, packet->iStartY);

	ReleaseDC(hWnd, hdc);
}

void ProcRead(SOCKET clientSocket)
{
	SESSION *p = FindSession(clientSocket);

	if (p == NULL)
		printf("!!!존재하지 않는 클라이언트!!!\n");

	char recvBuffer[5000];
	int ret = recv(clientSocket, recvBuffer, 5000, 0);
	if (ret == SOCKET_ERROR)
		return;
	ret = p->recvQueue.Enqueue(recvBuffer, ret);

	//recvQueue로 바로 수신
	//int ret = recv(p->Socket, (char*)&(p->recvQueue), );

	if (ret == 0)
		RemoveSession(clientSocket);

	unsigned short Header;
	st_DRAW_PACKET Packet;
	
	while (true)
	{
		//해당 세션의 RecvQ 에 들어있는 완성된  메시지 확인
		//헤더의 길이보다 작으면 아직 덜왔다 판단 후 나가기
		if (p->recvQueue.GetUseSize() <= 2) break;

		p->recvQueue.Peek((char*)&Header, 2);

		//패킷 하나가 완성이 안됐으면(헤더에 담긴 패킷 길이 + 헤더 크기) 할 게 없으니 나가기
		if (p->recvQueue.GetUseSize() < Header + 2) break;

		//헤더 지나서 패킷만 읽음
		p->recvQueue.MoveFront(2);
		p->recvQueue.Dequeue((char*)&Packet, Header);

		SendBroadcast(&Header, &Packet);

		drawLine(hWnd, &Packet);
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_NETWORK:
	{
		WORD event = WSAGETSELECTEVENT(lParam);
		WORD error = WSAGETSELECTERROR(lParam);

		if (error)
		{
			// wParam -> Socket  
			RemoveSession(wParam);
			break;
		}

		switch (event)
		{
		case FD_ACCEPT:
			ProcAccept();
			break;

		case FD_READ:
			ProcRead(wParam);
			break;

		case FD_WRITE:
			SESSION* pSession;
			pSession = FindSession(wParam);
			pSession->sendFlag = true;
			ProcWrite(pSession);
			break;

		case FD_CLOSE:
			RemoveSession(wParam); 
			break;
		}
	}
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
