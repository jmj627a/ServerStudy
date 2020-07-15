#include <WinSock2.h>
#include <Windows.h>
#include <process.h>
#include <list>

#define MAX_THREAD 5

using namespace std;

#pragma comment(lib,"Ws2_32.lib")

#include "CPacket.h"
#include "RingBuf.h"

typedef struct SESSION {

	SOCKET		socket;
	SOCKADDR_IN sockAddr;
	OVERLAPPED	sendOverlap;
	OVERLAPPED	recvOverlap;
	RingBuf		SendQ;
	RingBuf		RecvQ;
	WSABUF		Sendbuf;
	WSABUF		Recvbuf;
	LONG			IOCount = 0;
	long		bSendFlag = false;
	DWORD		dwSendByte = 0;

};

#define dfSERVER_PORT	9000

SOCKET listen_sock;
list<SESSION *> g_SessionList;
CRITICAL_SECTION cs;
HANDLE hThread[MAX_THREAD];
void InitServer();
void ReleaseClient();

void RecvPost(SESSION *pClient);
void SendPost(SESSION *pClient);
SESSION *CreateClient();
unsigned __stdcall AcceptThread(LPVOID acceptArg);
unsigned __stdcall SendThread(LPVOID sendArg);
unsigned __stdcall IOCPWorkerThread(LPVOID workerArg);
void main()
{
	InitServer();

	InitializeCriticalSection(&cs);

	WaitForMultipleObjects(MAX_THREAD, hThread, TRUE, INFINITE);
}

void InitServer()
{
	HANDLE hcp;

	int retval;

	//윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return;

	//소켓
	listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET)	return;

	//입출력 완료 포트 생성
	hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (NULL == hcp)
		return;

	//쓰레드 생성
	DWORD dwThreadID[5];
	hThread[0] = (HANDLE)_beginthreadex(NULL, 0, AcceptThread, hcp, 0, (unsigned int *)&dwThreadID[0]);
	if (hThread == NULL)
		return;
	CloseHandle(hThread);

	hThread[1] = (HANDLE)_beginthreadex(NULL, 0, SendThread, hcp, 0, (unsigned int *)&dwThreadID[1]);
	if (hThread == NULL)
		return;
	CloseHandle(hThread);

	for (int iCnt = 2; iCnt < 5; iCnt++)
	{
		hThread[iCnt] = (HANDLE)_beginthreadex(NULL, 0, IOCPWorkerThread, hcp, 0, (unsigned int *)&dwThreadID[iCnt]);
		if (hThread == NULL)
			return;
		CloseHandle(hThread);
	}

	//bind
	SOCKADDR_IN serveraddr;
	memset(&serveraddr, 0, sizeof(SOCKADDR_IN));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(dfSERVER_PORT);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(SOCKADDR_IN));
	if (retval == SOCKET_ERROR)
		return;

	//listen
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
		return;
}

unsigned __stdcall AcceptThread(LPVOID acceptArg)
{
	HANDLE hcp = (HANDLE)acceptArg;
	SOCKADDR_IN sockaddr;
	int addrlen = sizeof(SOCKADDR_IN);

	int retval;

	SESSION *pClient = CreateClient();
	pClient->socket = accept(listen_sock, (SOCKADDR *)&sockaddr, &addrlen);
	if (pClient->socket == INVALID_SOCKET)
		return 1;

	g_SessionList.push_back(pClient);

	hcp = CreateIoCompletionPort((HANDLE)pClient->socket, hcp, (DWORD)pClient, 0);

	RecvPost(pClient);
}

/*
컨텐츠에서 Send 작업시 -> SendQ 에 넣음 -> SendThread 에서
모든 접속 클라이언트에 대해 SendQ 확인.
SendQ 데이터가 있고, SendFlag 가 false 인 클라이언트에 대해 Send 작업
SendQ 에서 메시지를 뽑아서 -> WSABUFF 인스턴스 선언 후 SendBuff 지정
SendFlag = true -> WSASend 호출
*/
unsigned __stdcall SendThread(LPVOID sendArg)
{
	list<SESSION *>::iterator cIter;

	while (1)
	{
		for (cIter = g_SessionList.begin(); cIter != g_SessionList.end(); ++cIter)
		{
			SESSION *pSessioin= (*cIter);

			if (pSessioin->SendQ.GetUseSize() > 0 && pSessioin->bSendFlag == FALSE)
			{
				WSABUF sendBuf;
				sendBuf.buf = pSessioin->SendQ.GetRearBufferPtr();
				sendBuf.len = pSessioin->SendQ.DirectDequeueSize();
				pSessioin->bSendFlag = TRUE;
				WSASend(pSessioin->socket, &sendBuf, 1, NULL, 0, &pSessioin->sendOverlap, NULL);
			}
		}

		Sleep(20);
	}
}

/*
WSABUF 에 RecvBuf 지정 -> WSARecv 호출.
WSARecv 완료 -> RecvQ 에서 완료된 패킷에 대한 로직 처리.
*/
unsigned __stdcall IOCPWorkerThread(LPVOID workerArg)
{
	DWORD dwTransferred = 0;

	HANDLE hcp = (HANDLE)workerArg;
	WSABUF RecvBuf;
	OVERLAPPED *pOverlapped = NULL;
	SESSION *pClient = NULL;

	int retval;

	while (1)
	{
		retval = GetQueuedCompletionStatus(hcp, &dwTransferred, (PULONG_PTR)pClient,
			(LPOVERLAPPED *)pOverlapped, INFINITE);

		if (0 == dwTransferred)
		{
			shutdown(pClient->socket, SD_BOTH);
		}

		//recv 처리
		else if (pOverlapped == &pClient->recvOverlap)
		{
			SendPost(pClient);
		}

		//send 처리
		else if (pOverlapped == &pClient->sendOverlap)
		{

		}

		if (0 == InterlockedDecrement(&pClient->IOCount))
			ReleaseClient();
	}
}

SESSION *CreateClient()
{
	SESSION *pClient = new SESSION;

	memset(&pClient->sendOverlap, 0, sizeof(OVERLAPPED));
	memset(&pClient->recvOverlap, 0, sizeof(OVERLAPPED));

	pClient->socket = INVALID_SOCKET;

	pClient->RecvQ.ClearBuffer();
	pClient->SendQ.ClearBuffer();

	pClient->bSendFlag = FALSE;
	pClient->IOCount = 0;

	return pClient;
}

void ReleaseClient()
{

}

void RecvPost(SESSION *pClient)
{
	int retval, iCount;
	WSABUF wBuf;

	wBuf.buf = pClient->RecvQ.GetRearBufferPtr();
	wBuf.len = pClient->RecvQ.DirectEnqueueSize();

	InterlockedIncrement(&pClient->IOCount);
	retval = WSARecv(pClient->socket, &wBuf, 1, NULL, 0, &pClient->recvOverlap, NULL);
	if (retval == SOCKET_ERROR)
	{
		if (GetLastError() != WSA_IO_PENDING)
		{
			if (0 == InterlockedDecrement(&pClient->IOCount))
				ReleaseClient();
			return;
		}
	}
}

void SendPost(SESSION *pClient)
{
	int retval, iCount;
	WSABUF wBuf;

	wBuf.buf = pClient->SendQ.GetRearBufferPtr();
	wBuf.len = pClient->SendQ.DirectEnqueueSize();

	InterlockedIncrement(&pClient->IOCount);
	retval = WSASend(pClient->socket, &wBuf, 1, NULL, 0, &pClient->sendOverlap, NULL);
	if (retval == SOCKET_ERROR)
	{
		if (GetLastError() != WSA_IO_PENDING)
		{
			if (0 == InterlockedDecrement(&pClient->IOCount))
				ReleaseClient();
			return;
		}
	}
}