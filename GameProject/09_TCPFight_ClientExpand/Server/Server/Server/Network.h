#pragma once
#ifndef __NETWORK__
#define __NETWORK__
#include "protocol.h"
#include "RingBuf.h"
#include "ServerPrint.h"
#include "Content.h"

class CPacket;
struct st_SECTOR_POS;

//Ŭ���̾�Ʈ���� ���� ����. ������X ��Ʈ��ũO
struct st_SESSION
{
	SOCKET		socket;				//�� ������ TCP ����
	DWORD		dwSessionID;		//�������� ���� ���� ID
	RingBuf		RecvQ;				//���� ť
	RingBuf		SendQ;				//�۽� ť
	DWORD		dwRecvTime;			//�޼��� ���� üũ�� ���� �ð� (Ÿ�Ӿƿ���)
	DWORD		dwRecvCount;		//���ŵ� �޼��� �� ī���� (TPS ����)
	DWORD		dwRecvSecondTick;	//���� TPS Ȯ�ο� TICK

};

void netInit();
int netStartUp();
void netCleanUp();

//Socket ���� ���� ã��
st_SESSION *FindSession(SOCKET socket);

//���ο� ������ ����,��� �Ѵ�
st_SESSION *CreateSession(SOCKET socket);

//�ش� ���� ���� ó��
void DisconnectSession(SOCKET socket);

//���� ���� ��Ʈ��ũ ó�� �Լ�
void netIOProcess(void);

//select �� üũ �Լ�
void netSelectSocket(SOCKET *pTableSocket, FD_SET *pReadSet, FD_SET *pWriteSet);

//Recv ó��
void netRecvProc(SOCKET socket);

//Send ó��
bool netSendProc(SOCKET socket);

//����� ���� �̺�Ʈ ó��
bool netAcceptProc(void);

//��Ŷ�� �Ϸ�Ǿ����� Ȯ�� �� �Ϸ� �Ǿ��ٸ� ��Ŷ�� ó��
int CompleteRecvPacket(st_SESSION *pSession);

//��Ŷ Ÿ�Կ� ���� ó�� �Լ� ȣ�� (����, ��ŶŸ��, ����ȭ����)
bool PacketProc(st_SESSION *pSession, BYTE byPacketType, CPacket *pPacket);

//����ڿ��� �Ѹ���
void SendPacket_Around(st_SESSION *pSession, CPacket *pPacket, bool includePlayer = false);
void SendPacket_Unicast(st_SESSION *pSession, CPacket *pPacket);
void SendPacket_Broadcast(st_SESSION *pSession, CPacket *pPacket);
void SendPacket_SectorOne(st_SESSION *pSession, CPacket *pPacket, st_SECTOR_POS *pPos);// , bool includePlayer = false);

#endif