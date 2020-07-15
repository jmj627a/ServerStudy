#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <process.h>
#include <list>
#include "RingBuf.h"
#include "CPacket.h"

#pragma comment(lib,"ws2_32")
#pragma comment(lib,"winmm")

#define dfSERVER_PORT	9000
#define dfMAX_BUFFER	10000
#define dfMAX_WORKER	4
#define dfMAX_THREAD	(dfMAX_WORKER + 1)
#define dfMAX_TIMEOUT	300000
#define dfMAX_PACKET	512

using namespace std;

typedef struct SESSION {

	SOCKET		socket;
	SOCKADDR_IN sockAddr;
	OVERLAPPED	sendOverlap;
	OVERLAPPED	recvOverlap;
	RingBuf		SendQ;
	RingBuf		RecvQ;
	WSABUF		Sendbuf;
	WSABUF		Recvbuf;
	bool		bSendFlag;

};

SOCKET g_listenSock;
HANDLE g_iocp;
HANDLE g_Shutdown;
list<SESSION *> g_SessionList;

SRWLOCK g_srwSession;

unsigned int __stdcall Accept_Thread(void *args);
unsigned int __stdcall Worker_Thread(void *args);
bool DeleteSession(SESSION *pSession);

int main()
{
	// beginPeriod
	timeBeginPeriod(1);

	// setlocale
	setlocale(LC_ALL, " ");

	// initial Critical
	InitializeSRWLock(&g_srwSession);

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
	int retval = bind(g_listenSock, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
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
	hThread[0] = (HANDLE)_beginthreadex(NULL, 0, Accept_Thread, NULL, 0, (unsigned int *)&dwThreadID[0]);
	for (int i = 1; i < dfMAX_THREAD; i++)
		hThread[i] = (HANDLE)_beginthreadex(NULL, 0, Worker_Thread, NULL, 0, (unsigned int *)&dwThreadID[i]);
	for (int i = 0; i < dfMAX_THREAD; i++) {
		if (hThread[i] == NULL)
			return 1;

	}

	// CreateEvent
	g_Shutdown = CreateEvent(NULL, FALSE, FALSE, NULL);

	while(true)
	{
		// SPACE 키가 눌렸다면 종료
		if (GetAsyncKeyState(VK_SPACE))
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

unsigned int __stdcall Accept_Thread(void *args)
{
	SOCKET Socket;
	SOCKADDR_IN clientAddr;
	int clientAddrlen = sizeof(clientAddr);
	DWORD		dwRecvbyte;
	DWORD		dwFlag;
	WCHAR szIP[16];

	wprintf(L"acept thread strart \n");
	while (g_Shutdown)
	{

		Socket = accept(g_listenSock, (SOCKADDR*)&clientAddr, &clientAddrlen);
		if (Socket == INVALID_SOCKET)	//bind() or listen_socket 에러시
		{
			break;
		}

		inet_ntop(AF_INET, &clientAddr.sin_addr, (PSTR)szIP, clientAddrlen);
		wprintf(L"[TCP 서버] 클라이언트 접속 : IP 주소 = %s, 포트 번호 = %d", szIP, ntohs(clientAddr.sin_port));

		SESSION  *pSession = new SESSION;
		pSession->socket = Socket;
		pSession->sockAddr = clientAddr;
		pSession->bSendFlag = false;
		pSession->Recvbuf.buf = pSession->RecvQ.GetRearBufferPtr();
		pSession->Recvbuf.len = pSession->RecvQ.DirectEnqueueSize();
		pSession->Sendbuf.buf = pSession->SendQ.GetFrontBufferPtr();
		pSession->Sendbuf.len = pSession->SendQ.DirectDequeueSize();
		memset(&pSession->recvOverlap, 0, sizeof(OVERLAPPED));
		memset(&pSession->sendOverlap, 0, sizeof(OVERLAPPED));


		CreateIoCompletionPort((HANDLE)Socket, g_iocp, (ULONG_PTR)pSession, 0);
		ZeroMemory(&pSession->recvOverlap, sizeof(pSession->recvOverlap));
		dwRecvbyte = 0;
		dwFlag = 0;

		AcquireSRWLockExclusive(&g_srwSession);
		g_SessionList.push_back(pSession);
		ReleaseSRWLockExclusive(&g_srwSession);

		int ret = WSARecv(pSession->socket, &pSession->Recvbuf, 1, &dwRecvbyte, &dwFlag, &pSession->recvOverlap, NULL);
		

		if (ret == SOCKET_ERROR) 
		{

			int iError = WSAGetLastError();
			if (iError != WSA_IO_PENDING) {

				closesocket(Socket);
				delete pSession;
			}
			continue;
		}

	}

	wprintf(L"acept thread end \n");
	_endthreadex(0);

	return 0;

}

unsigned int __stdcall Worker_Thread(void *args)
{

	DWORD dwTransfer;
	SESSION *pSession;
	OVERLAPPED *pOverlap;
	DWORD dwSendByte;
	DWORD dwRecvByte;
	DWORD dwFlag;

	printf("Enter worker Thread\n");

	while(true)
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
		if (pOverlap == &pSession->sendOverlap) 
		{
			pSession->SendQ.MoveFront(dwTransfer);
			//
			//dwSendByte = 0;
			//
			//pSession->Sendbuf.buf = pSession->SendQ.GetFrontBufferPtr();
			//pSession->Sendbuf.len = pSession->SendQ.DirectDequeueSize();
			//
			//pSession->bSendFlag = true;
			//
			//int iRet = WSASend(pSession->socket, &pSession->Sendbuf, 1, &dwSendByte, 0, &pSession->sendOverlap, NULL);
			//if (iRet == SOCKET_ERROR && WSAGetLastError() != ERROR_IO_PENDING) {
			//
			//	shutdown(pSession->socket, SD_BOTH);
			//	continue;
			//}
			//
			//pSession->bSendFlag = false;
			//
			//// MoveFront
			//pSession->SendQ.MoveFront(dwSendByte);

		}
		else if (pOverlap == &pSession->recvOverlap)
		{
			pSession->RecvQ.MoveRear(dwTransfer);

			//while(1)
			{
				// init
				int recvSize = pSession->RecvQ.GetUseSize();
				int RetDeqSize = 0;

				RetDeqSize = pSession->RecvQ.Dequeue(pSession->SendQ.GetRearBufferPtr(), pSession->RecvQ.GetUseSize());
				pSession->SendQ.MoveRear(RetDeqSize);

				pSession->Sendbuf.buf = pSession->SendQ.GetFrontBufferPtr();
				pSession->Sendbuf.len = pSession->SendQ.DirectDequeueSize();


				memset(&pSession->sendOverlap, 0, sizeof(OVERLAPPED));

				int ret = WSASend(pSession->socket, &pSession->Sendbuf, 1, &dwSendByte, 0, &pSession->sendOverlap, NULL);

				int error = WSAGetLastError();
				if (ret == SOCKET_ERROR && error != ERROR_IO_PENDING)
				{
					shutdown(pSession->socket, SD_BOTH);
					return 0;
				}
			}

			dwRecvByte = 0;
			dwFlag = 0;

			pSession->Recvbuf.buf = pSession->RecvQ.GetRearBufferPtr();
			pSession->Recvbuf.len = pSession->RecvQ.DirectEnqueueSize();
			int iRet = WSARecv(pSession->socket, &pSession->Recvbuf, 1, &dwRecvByte, &dwFlag, &pSession->recvOverlap, NULL);
			if (iRet == SOCKET_ERROR && WSAGetLastError() != ERROR_IO_PENDING) {

				shutdown(pSession->socket, SD_BOTH);
				continue;
			}

			// MoveRear
			//pSession->RecvQ.MoveRear(dwRecvByte);
		}
	}

	printf("Exit worker Thread\n");

	// 스레드 연계 종료
	PostQueuedCompletionStatus(g_iocp, 0, NULL, NULL);

	return 0;
}

bool DeleteSession(SESSION *pSession) {

	bool bRet = false;

	AcquireSRWLockExclusive(&g_srwSession);

	list<SESSION *>::iterator SessionIter = g_SessionList.begin();
	list<SESSION *>::iterator SessionIterEnd = g_SessionList.end();

	for (; SessionIter != SessionIterEnd; ++SessionIter) {

		if (pSession == (*SessionIter)) {

			//delete
			g_SessionList.erase(SessionIter);
			bRet = true;
			break;
		}
	}
	ReleaseSRWLockExclusive(&g_srwSession);

	return bRet;
}