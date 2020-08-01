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

	//overlap io �� ���� ���� �ʿ��� ��ü 
	OVERLAPPED	sendOverlap;
	OVERLAPPED	recvOverlap;

	//�Ϸ����� �ö����� ����
	RingBuf		SendQ;
	RingBuf		RecvQ;
	
	//
	WSABUF		Sendbuf;
	WSABUF		Recvbuf;

	//���� send���̸� ���ۿ� �ְ� ����
	//������ send �Ϸ����� ������ sendq�� �ִ°� sendpost ȣ��.
	long		sendFlag;

	//���� ������ ����ȭ ��ü
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

//wsaRecv �Լ� ����
void recvPost(int iSessionID);

//wsaSend �Լ� ����
void sendPost(int iSessionID);

//������ �ִ�. �޼��� ������ �̾Ƴ��Ŵ�. ť ����������� ��� �޼��� ������ �̾Ƽ� ȣ��. ��� ���� �˸��̸�
void onRecv(int iSessionID, CPacket* pPacket);

//�����⸦ �� �ߴ�. ���� �� ���� ���µ� ¦���߱� ���� ����. �� �������� �Ϲ������� �������� �˷��� �ʿ� ����.
void onSend();

//� ������ ���Դ�. id�� Ȯ���ؼ� ������������ �˷��ִ� ����. acceptthread���� ���� �� �ϰ� �������� ȣ��.
void onAccept();

//���⼭ leave�� release���� �� �Ǿ��ٴ°�. �� ������ ���������� ���� release �Ǿ��ٴ°�. 
void onLeave(int iSessionID);

//���������� ������ �����ϱ� ���� ���� ��. �� �ȿ��� onleaveȣ��. ������ �� ����� Ȯ���ϰ�, iocount���� ����, ������ ������ �ؾ��ϴ� ���� onleave ȣ��
bool DeleteSession(SESSION* pSession);

//���� �˻�
SESSION* FindSession(int iSessionID);

//��Ŷ ���� sendQ�� ��� sendPost ȣ��
void SendPacket(int iSessionID, CPacket* pPacket);

//�긦 ����޶�
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

	//send���̶�� flag�� �ٲ�. �̹� ���� �ϰ��ִٸ� �׳� ������. (���� �۾��� �Ϸ����� ���� ��)
	if (InterlockedCompareExchange(&session->sendFlag, 1, 0) == 1)
		return;

	//wsaSend ����
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
		//��¥ ����
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

//wsaRecv �Լ� ����
void recvPost(int iSessionID)
{
	SESSION* session = FindSession(iSessionID);

	int ret;
	DWORD dwRecvByte = 0;
	DWORD dwFlag = 0;
	WSABUF wsaBuf[2];
	int iBufCount = 0;

	//wsabuf, overlapped �ʱ�ȭ
	ZeroMemory(&session->recvOverlap, sizeof(session->recvOverlap));
	wsaBuf[iBufCount].buf = session->RecvQ.GetRearBufferPtr();
	wsaBuf[iBufCount].len = session->RecvQ.DirectEnqueueSize();
	iBufCount++;

	//���� �ڿ� ©���� ���ο� ���ۿ� ���, �ᱹ �ΰ� �����͸� ���� �Ѱܼ� �̾�޵��� ����
	if (session->RecvQ.DirectEnqueueSize() < session->RecvQ.GetFreeSize())
	{
		//�������� ���ۺκ� �����͸� ����
		wsaBuf[iBufCount].buf = session->RecvQ.GetBufferPtr();
		wsaBuf[iBufCount].len = session->RecvQ.GetFreeSize() - session->RecvQ.DirectEnqueueSize();
		iBufCount++;
	}

	//recv �Լ� ȣ�⿡ ���� iocount ���� 
	InterlockedIncrement(&session->ioCount);
	ret = WSARecv(session->socket, wsaBuf, iBufCount, &dwRecvByte, &dwFlag, &session->recvOverlap, nullptr);

	if(ret == SOCKET_ERROR)
	{
		int error = WSAGetLastError();
		//��¥ ����
		if (WSA_IO_PENDING != error)
		{
			wprintf(L"recvPost WSARecv error - %d \n", error);

			//iocount ����, 0���� üũ
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
		// esc Ű�� ���ȴٸ� ����
		if (GetAsyncKeyState(VK_ESCAPE))
		{
			// Update ����
			SetEvent(g_Shutdown);

			// Accept ����
			closesocket(g_listenSock);

			// Post����� ��
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
		if (Socket == INVALID_SOCKET)	//bind() or listen_socket ������
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
		pSession->ioCount = 0; //�Ϸ����� �� ���Ұ� �ƴ϶� �����ִٰ� workerthread �� ���������� ����

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

		//GQCS �������� ���
		GetQueuedCompletionStatus(g_iocp, &dwTransfer, (PULONG_PTR)&pSession, &pOverlap, INFINITE);

		// PostQueuedCompletionStatus�� ���� ������ ����
		if (dwTransfer == 0 && pSession == NULL && pOverlap == NULL)
			break;

		if (pOverlap == NULL)
			continue;

		if (pSession == NULL)
			continue;

		// ��������ó��
		if (dwTransfer == 0)
		{
			closesocket(pSession->socket);
			bool bRet = DeleteSession(pSession);
			if (bRet)
				delete pSession;
			continue;
		}

		// ���� send ���� recv ���� �Ǵ�
		//send �Ϸ����� 
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
		//recv �Ϸ�����
		else if (pOverlap == &pSession->recvOverlap)
		{
			//�޾��� ��ŭ ť �̵� 
			pSession->RecvQ.MoveRear(dwTransfer);

			//�޾����� �� ó���Ҷ�����
			while(true)
			{
				// ���� ��Ŷ Ȯ��
				int recvSize = pSession->RecvQ.GetUseSize();

				//��Ŷ ������ �˻�. ������� ������ ����
				if (sizeof(short) > recvSize)
					break;

				//��Ŷ�� ���� �Դ��� Ȯ��. ��� + ���̷ε� ������ 
				WCHAR header;
				pSession->RecvQ.Peek((char*)&header, sizeof(short));
				//if (header != 0x08)
				//	break; 
				if (sizeof(short) + header > recvSize)
					break;
				//��� �κ��� ���� onRecv�� ����.
				pSession->RecvQ.MoveFront(sizeof(short));

				CPacket packet;

				//��� ũ�⸸ŭ ����ȭ���۷� �̾Ƴ� 
				if (header != pSession->RecvQ.Dequeue(packet.GetWritePtr(), header))
					break;
				packet.MoveWritePos(header);

				//�� �ȿ��� sendpacket ȣ���ϰ� ��Ŷ ��������
				onRecv(pSession->sessionID, &packet);

				//packet.Clear();
			}


			//���� �� ������ �ٽ� recv �ɰ� ������ 
			recvPost(pSession->sessionID);
		}


		if (InterlockedDecrement(&pSession->ioCount) == 0)
		{
			//ReleaseIO(pSession);
		}
	}

	printf("Exit worker Thread\n");

	// ������ ���� ����
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
