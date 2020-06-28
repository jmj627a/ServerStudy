#pragma once
#ifndef __NETWORK__
#define __NETWORK__
#include "protocol.h"
#include "RingBuf.h"
#include "ServerPrint.h"
#include "Content.h"

class CPacket;
struct st_SECTOR_POS;

//클라이언트들의 접속 정보. 컨텐츠X 네트워크O
struct st_SESSION
{
	SOCKET		socket;				//현 접속의 TCP 소켓
	DWORD		dwSessionID;		//접속자의 고유 세션 ID
	RingBuf		RecvQ;				//수신 큐
	RingBuf		SendQ;				//송신 큐
	DWORD		dwRecvTime;			//메세지 수신 체크를 위한 시간 (타임아웃용)
	DWORD		dwRecvCount;		//수신된 메세지 수 카운터 (TPS 계산용)
	DWORD		dwRecvSecondTick;	//수신 TPS 확인용 TICK

};

void netInit();
int netStartUp();
void netCleanUp();

//Socket 으로 세션 찾기
st_SESSION *FindSession(SOCKET socket);

//새로운 세션을 생성,등록 한다
st_SESSION *CreateSession(SOCKET socket);

//해당 세션 종료 처리
void DisconnectSession(SOCKET socket);

//서버 메인 네트워크 처리 함수
void netIOProcess(void);

//select 모델 체크 함수
void netSelectSocket(SOCKET *pTableSocket, FD_SET *pReadSet, FD_SET *pWriteSet);

//Recv 처리
void netRecvProc(SOCKET socket);

//Send 처리
bool netSendProc(SOCKET socket);

//사용자 접속 이벤트 처리
bool netAcceptProc(void);

//패킷이 완료되었는지 확인 후 완료 되었다면 패킷을 처리
int CompleteRecvPacket(st_SESSION *pSession);

//패킷 타입에 따른 처리 함수 호출 (세션, 패킷타입, 직렬화버퍼)
bool PacketProc(st_SESSION *pSession, BYTE byPacketType, CPacket *pPacket);

//사용자에게 뿌리기
void SendPacket_Around(st_SESSION *pSession, CPacket *pPacket, bool includePlayer = false);
void SendPacket_Unicast(st_SESSION *pSession, CPacket *pPacket);
void SendPacket_Broadcast(st_SESSION *pSession, CPacket *pPacket);
void SendPacket_SectorOne(st_SESSION *pSession, CPacket *pPacket, st_SECTOR_POS *pPos);// , bool includePlayer = false);

#endif