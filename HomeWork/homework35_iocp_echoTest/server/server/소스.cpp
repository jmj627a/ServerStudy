#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <process.h>
#include <list>

#include "RingBuf.h"
#include "CPacket.h"

#pragma comment(lib,"ws2_32")
#pragma comment(lib,"winmm")

#define dfSERVER_PORT	6000
#define dfMAX_BUFFER	10000
#define dfMAX_WORKER	4
#define dfMAX_THREAD	(dfMAX_WORKER + 1)
#define dfMAX_TIMEOUT	300000
#define dfMAX_PACKET	512

using namespace std;

typedef struct SESSION {

	SOCKET		socket;
	SOCKADDR_IN sockAddr;

	//overlap io 를 쓰기 위해 필요한 객체 
	OVERLAPPED	sendOverlap;
	OVERLAPPED	recvOverlap;

	//완료통지 올때까지 보존
	RingBuf		SendQ;
	RingBuf		RecvQ;
	
	//
	WSABUF		Sendbuf;
	WSABUF		Recvbuf;

	//지금 send중이면 버퍼에 넣고 빠짐
	//다음번 send 완료통지 왔을때 sendq에 있는거 sendpost 호출.
	long		sendFlag;

	//세션 각각의 동기화 객체
	CRITICAL_SECTION cs;

	int sessionID;

	long ioCount;

	RingBuf saveBuf;
};

CPacket *g_tempPacket = new CPacket(1000000);
long g_insize = 0;
CRITICAL_SECTION g_CS_packet;

SOCKET g_listenSock;
HANDLE g_iocp;
HANDLE g_Shutdown;

list<SESSION*> g_SessionList;
CRITICAL_SECTION g_CS_sessionlist;

long g_sessionIDCount = -1;

unsigned int __stdcall Accept_Thread(void* args);
unsigned int __stdcall Worker_Thread(void* args);

//wsaRecv 함수 랩핑
void recvPost(int iSessionID);

//wsaSend 함수 랩핑
void sendPost(int iSessionID);

//받을게 있다. 메세지 단위로 뽑아낼거다. 큐 비워질때까지 계속 메세지 단위로 뽑아서 호출. 헤더 빼고 알맹이만
void onRecv(int iSessionID, CPacket* pPacket);

//보내기를 다 했다. 원래 할 일은 없는데 짝맞추기 위해 만듦. 다 보낸것을 일반적으로 컨텐츠에 알려줄 필요 없음.
void onSend();

//어떤 세션이 들어왔다. id를 확보해서 컨텐츠쪽으로 알려주는 역할. acceptthread에서 세팅 다 하고 마지막에 호출.
void onAccept();

//여기서 leave는 release까지 다 되었다는거. 이 세션이 최종적으로 정말 release 되었다는것. 
void onLeave(int iSessionID);

//실질적으로 세션을 해제하기 위해 들어가는 것. 이 안에서 onleave호출. 정말로 다 끊긴거 확인하고, iocount까지 보고, 정말로 릴리즈 해야하는 순간 onleave 호출
bool DeleteSession(SESSION* pSession);

//세션 검색
SESSION* FindSession(int iSessionID);

//패킷 만들어서 sendQ에 담고 sendPost 호출
void SendPacket(int iSessionID, CPacket* pPacket);

//얘를 끊어달라
void Disconnect(int iSessionID);


ULONG firstsize = 0;
ULONG secoundsize = 0;

void SendPacket(int iSessionID, CPacket* pPacket)
{
	SESSION* session = FindSession(iSessionID);
	if (session == nullptr)
		return;


	WCHAR header = pPacket->GetDataSize();
	if (header != 0x08)
		return;

	CPacket packet;
	packet.PutData((char*)&header, sizeof(header));
	packet.PutData(pPacket->GetReadPtr(), header);

	////*******
	EnterCriticalSection(&g_CS_packet);

	EnterCriticalSection(&session->cs);
	int ret1 = session->saveBuf.Enqueue((char*)&header, sizeof(header));
	int ret2 = session->saveBuf.Enqueue(pPacket->GetReadPtr(), header);
	int size = session->SendQ.Enqueue(packet.GetReadPtr(), packet.GetDataSize());
	LeaveCriticalSection(&session->cs);
	
	LeaveCriticalSection(&g_CS_packet);
	//****************

	//EnterCriticalSection(&session->cs);
	//int size = session->SendQ.Enqueue(packet.GetReadPtr(), packet.GetDataSize());
	//LeaveCriticalSection(&session->cs);

	if (size <= 0)
		int a = 0;


	//InterlockedAdd(&g_insize, sizeof(header) + header);

	sendPost(session->sessionID);
}


void onRecv(int iSessionID, CPacket* pPacket)
{
	LONGLONG temp;
	*pPacket >> temp;

	CPacket sendPacket;
	sendPacket << temp;

	SendPacket(iSessionID, &sendPacket);
}

void sendPost(int iSessionID)
{
	SESSION* session = FindSession(iSessionID);

	//send중이라고 flag를 바꿈. 이미 누가 하고있다면 그냥 나가기. (남은 작업은 완료통지 받은 후)
	if (InterlockedCompareExchange(&session->sendFlag, 1, 0) == 1)
		return;

	//wsaSend 실행
	if (session->SendQ.GetUseSize() == 0)
	{
		InterlockedDecrement(&session->sendFlag);
		return;
	}

	int ret;
	DWORD dwSendByte = 0;
	DWORD dwFlag = 0;
	WSABUF wsaBuf[2] = { 0, };
	int iBufCount = 0;

	ZeroMemory(&session->sendOverlap, sizeof(session->sendOverlap));

	EnterCriticalSection(&session->cs);
	wsaBuf[0].buf = session->SendQ.GetFrontBufferPtr();
	wsaBuf[0].len = session->SendQ.DirectDequeueSize();
	if (wsaBuf[0].len > 10000)
		wsaBuf[0].len = session->SendQ.DirectDequeueSize();
	firstsize = wsaBuf[0].len;
	iBufCount++;

	if (wsaBuf[0].len > 10000)
		int a = 0;

	if (session->SendQ.DirectDequeueSize() < session->SendQ.GetUseSize())
	{
		wsaBuf[iBufCount].buf = session->SendQ.GetBufferPtr();
		wsaBuf[iBufCount].len = session->SendQ.GetUseSize() - session->SendQ.DirectDequeueSize();
		secoundsize = wsaBuf[iBufCount].len;
		iBufCount++;
	}
	LeaveCriticalSection(&session->cs);


	if (wsaBuf[0].len > 10000)
		int a = 0;

	InterlockedIncrement(&session->ioCount);
	
	if (wsaBuf[0].len > 10000)
		int a = 0;

	ret = WSASend(session->socket, wsaBuf, iBufCount, &dwSendByte, 0, &session->sendOverlap, NULL);
	if (ret == SOCKET_ERROR)
	{
		int error = WSAGetLastError();
		//진짜 에러
		if (error != WSA_IO_PENDING)
		{
			wprintf(L"sendPost WSASend error - %d \n", error);
			
			InterlockedDecrement(&session->sendFlag);
			if (InterlockedDecrement(&session->ioCount) == 0)
			{
				//ReleaseIO(session);
			}
			return;
		}
	}
	//LeaveCriticalSection(&session->cs);
}

//wsaRecv 함수 랩핑
void recvPost(int iSessionID)
{
	SESSION* session = FindSession(iSessionID);

	int ret;
	DWORD dwRecvByte = 0;
	DWORD dwFlag = 0;
	WSABUF wsaBuf[2];
	int iBufCount = 0;

	//wsabuf, overlapped 초기화
	ZeroMemory(&session->recvOverlap, sizeof(session->recvOverlap));
	wsaBuf[iBufCount].buf = session->RecvQ.GetRearBufferPtr();
	wsaBuf[iBufCount].len = session->RecvQ.DirectEnqueueSize();
	iBufCount++;

	//버퍼 뒤에 짤리면 새로운 버퍼에 담아, 결국 두개 포인터를 같이 넘겨서 이어받도록 보냄
	if (session->RecvQ.DirectEnqueueSize() < session->RecvQ.GetFreeSize())
	{
		//링버퍼의 시작부분 포인터를 담음
		wsaBuf[iBufCount].buf = session->RecvQ.GetBufferPtr();
		wsaBuf[iBufCount].len = session->RecvQ.GetFreeSize() - session->RecvQ.DirectEnqueueSize();
		iBufCount++;
	}

	//recv 함수 호출에 대한 iocount 증가 
	InterlockedIncrement(&session->ioCount);
	ret = WSARecv(session->socket, wsaBuf, iBufCount, &dwRecvByte, &dwFlag, &session->recvOverlap, nullptr);

	if(ret == SOCKET_ERROR)
	{
		int error = WSAGetLastError();
		//진짜 에러
		if (WSA_IO_PENDING != error)
		{
			wprintf(L"recvPost WSARecv error - %d \n", error);

			//iocount 감소, 0인지 체크
			if (InterlockedDecrement(&session->ioCount) == 0)
			{

			}
		}
	}

	//LeaveCriticalSection(&session->cs);
}



int main()
{
	// beginPeriod
	timeBeginPeriod(1);

	// setlocale
	setlocale(LC_ALL, " ");

	// initial Critical
	InitializeCriticalSection(&g_CS_sessionlist);
	InitializeCriticalSection(&g_CS_packet);

	// win start
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// IOCP Create
	g_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (g_iocp == NULL)
		return 1;

	// socket
	g_listenSock = socket(AF_INET, SOCK_STREAM, 0);
	if (g_listenSock == INVALID_SOCKET)
		return 1;

	// bind
	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(dfSERVER_PORT);
	//InetPton(AF_INET, L"0.0.0.0", &serverAddr.sin_addr);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	int retval = bind(g_listenSock, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (retval == SOCKET_ERROR) {
		printf("bind()\n");
		return 1;
	}

	// listen
	retval = listen(g_listenSock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
		return 1;

	// Create Thread
	HANDLE hThread[dfMAX_THREAD];
	DWORD dwThreadID[dfMAX_THREAD];
	hThread[0] = (HANDLE)_beginthreadex(NULL, 0, Accept_Thread, NULL, 0, (unsigned int*)&dwThreadID[0]);
	for (int i = 1; i < dfMAX_THREAD; i++)
		hThread[i] = (HANDLE)_beginthreadex(NULL, 0, Worker_Thread, NULL, 0, (unsigned int*)&dwThreadID[i]);
	for (int i = 0; i < dfMAX_THREAD; i++) {
		if (hThread[i] == NULL)
			return 1;

	}

	// CreateEvent
	g_Shutdown = CreateEvent(NULL, FALSE, FALSE, NULL);

	while (true)
	{
		// esc 키가 눌렸다면 종료
		if (GetAsyncKeyState(VK_ESCAPE))
		{
			// Update 종료
			SetEvent(g_Shutdown);

			// Accept 종료
			closesocket(g_listenSock);

			// Post쳐줘야 함
			PostQueuedCompletionStatus(g_iocp, 0, NULL, NULL);
		}

		DWORD dwRet = WaitForMultipleObjects(dfMAX_THREAD, hThread, true, 500);
		if (dwRet != WAIT_TIMEOUT) {
			break;
		}
	}

	// closehandle
	for (int i = 0; i < dfMAX_THREAD; i++)
		CloseHandle(hThread[i]);

	// WSACleanup
	WSACleanup();

	// EndPeriod
	timeEndPeriod(1);

	return 0;
}

unsigned int __stdcall Accept_Thread(void* args)
{
	SOCKET Socket;
	SOCKADDR_IN clientAddr;
	int clientAddrlen = sizeof(clientAddr);
	DWORD dwRecvByte = 0;
	DWORD dwFlag = 0;
	WCHAR szIP[16];

	wprintf(L"acept thread strart \n");
	while (g_Shutdown)
	{

		Socket = accept(g_listenSock, (SOCKADDR*)&clientAddr, &clientAddrlen);
		if (Socket == INVALID_SOCKET)	//bind() or listen_socket 에러시
		{
			break;
		}

		//inet_ntop(AF_INET, &clientAddr.sin_addr, (PSTR)szIP, clientAddrlen);

		SESSION* pSession = new SESSION;
		InitializeCriticalSection(&pSession->cs);
		pSession->socket = Socket;
		pSession->sockAddr = clientAddr;
		pSession->sendFlag = false;
		pSession->Recvbuf.buf = pSession->RecvQ.GetRearBufferPtr();
		pSession->Recvbuf.len = pSession->RecvQ.DirectEnqueueSize();
		pSession->Sendbuf.buf = pSession->SendQ.GetFrontBufferPtr();
		pSession->Sendbuf.len = pSession->SendQ.DirectDequeueSize();
		pSession->sessionID = InterlockedIncrement(&g_sessionIDCount);
		pSession->ioCount = 0; //완료통지 후 감소가 아니라 물고있다가 workerthread 맨 마지막에서 감소

		memset(&pSession->recvOverlap, 0, sizeof(OVERLAPPED));
		memset(&pSession->sendOverlap, 0, sizeof(OVERLAPPED));


		ZeroMemory(&pSession->recvOverlap, sizeof(pSession->recvOverlap));
		dwRecvByte = 0;
		dwFlag = 0;

		EnterCriticalSection(&g_CS_sessionlist);
		g_SessionList.push_back(pSession);
		LeaveCriticalSection(&g_CS_sessionlist);

		CreateIoCompletionPort((HANDLE)Socket, g_iocp, (ULONG_PTR)pSession, 0);
		recvPost(pSession->sessionID);
	}

	wprintf(L"acept thread end \n");
	_endthreadex(0);

	return 0;

}

//
unsigned int __stdcall Worker_Thread(void* args)
{

	DWORD dwTransfer;
	SESSION* pSession;
	OVERLAPPED* pOverlap;
	DWORD dwSendByte = 0;
	DWORD dwRecvByte = 0;
	DWORD dwFlag = 0;

	printf("Enter worker Thread\n");

	while (true)
	{
		dwTransfer = 0;
		pSession = NULL;
		pOverlap = NULL;

		//GQCS 기준으로 블락
		GetQueuedCompletionStatus(g_iocp, &dwTransfer, (PULONG_PTR)&pSession, &pOverlap, INFINITE);

		// PostQueuedCompletionStatus에 의한 스레드 종료
		if (dwTransfer == 0 && pSession == NULL && pOverlap == NULL)
			break;

		if (pOverlap == NULL)
			continue;

		if (pSession == NULL)
			continue;

		// 세션종료처리
		if (dwTransfer == 0)
		{
			closesocket(pSession->socket);
			bool bRet = DeleteSession(pSession);
			if (bRet)
				delete pSession;
			continue;
		}

		// 이제 send 인지 recv 인지 판단
		//send 완료통지 
		if (pOverlap == &pSession->sendOverlap)
		{
			EnterCriticalSection(&g_CS_packet);

			char* pQueue = pSession->SendQ.GetFrontBufferPtr();
			char* pPacket = pSession->saveBuf.GetFrontBufferPtr();
			for (int i = 0; i < dwTransfer; i++)
			{
				if (*(pQueue + i) != *(pPacket + i))
				{
					int a = 0;
				}
			}
			
			EnterCriticalSection(&pSession->cs);
			pSession->saveBuf.MoveFront(dwTransfer);
			pSession->SendQ.MoveFront(dwTransfer);
			LeaveCriticalSection(&pSession->cs);

			LeaveCriticalSection(&g_CS_packet);


			firstsize   = 0;
			secoundsize = 0;

			InterlockedDecrement(&pSession->sendFlag);
			sendPost(pSession->sessionID);
		}
		//recv 완료통지
		else if (pOverlap == &pSession->recvOverlap)
		{
			//받아진 만큼 큐 이동 
			pSession->RecvQ.MoveRear(dwTransfer);

			//받아진거 다 처리할때까지
			while(true)
			{
				// 받은 패킷 확인
				int recvSize = pSession->RecvQ.GetUseSize();

				//패킷 사이즈 검사. 헤더보다 작으면 리턴
				if (sizeof(short) > recvSize)
					break;

				//패킷이 전부 왔는지 확인. 헤더 + 페이로드 사이즈 
				WCHAR header;
				pSession->RecvQ.Peek((char*)&header, sizeof(short));
				//if (header != 0x08)
				//	break; 
				if (sizeof(short) + header > recvSize)
					break;
				//헤더 부분은 빼서 onRecv로 전달.
				pSession->RecvQ.MoveFront(sizeof(short));

				CPacket packet;

				//헤더 크기만큼 직렬화버퍼로 뽑아냄 
				if (header != pSession->RecvQ.Dequeue(packet.GetWritePtr(), header))
					break;
				packet.MoveWritePos(header);

				//이 안에서 sendpacket 호출하고 패킷 보내버림
				onRecv(pSession->sessionID, &packet);

				//packet.Clear();
			}


			//할일 다 했으면 다시 recv 걸고 나가기 
			recvPost(pSession->sessionID);
		}


		if (InterlockedDecrement(&pSession->ioCount) == 0)
		{
			//ReleaseIO(pSession);
		}
	}

	printf("Exit worker Thread\n");

	// 스레드 연계 종료
	PostQueuedCompletionStatus(g_iocp, 0, NULL, NULL);

	return 0;
}

bool DeleteSession(SESSION* pSession) {

	bool bRet = false;

	EnterCriticalSection(&g_CS_sessionlist);

	list<SESSION*>::iterator SessionIter = g_SessionList.begin();
	list<SESSION*>::iterator SessionIterEnd = g_SessionList.end();

	for (; SessionIter != SessionIterEnd; ++SessionIter) {

		if (pSession == (*SessionIter)) {

			//delete
			EnterCriticalSection(&(*SessionIter)->cs);
			LeaveCriticalSection(&(*SessionIter)->cs);
			g_SessionList.erase(SessionIter);
			bRet = true;
			break;
		}
	}
	LeaveCriticalSection(&g_CS_sessionlist);

	return bRet;
}

SESSION* FindSession(int iSessionID)
{
	EnterCriticalSection(&g_CS_sessionlist);

	list<SESSION*>::iterator iter = g_SessionList.begin();
	list<SESSION*>::iterator enditer = g_SessionList.end();

	for (iter; iter != enditer; ++iter)
	{
		if ((*iter)->sessionID == iSessionID)
		{
			//EnterCriticalSection(&(*iter)->cs);
			LeaveCriticalSection(&g_CS_sessionlist);
			return *iter;
		}
	}

	LeaveCriticalSection(&g_CS_sessionlist);
	return nullptr;
}
