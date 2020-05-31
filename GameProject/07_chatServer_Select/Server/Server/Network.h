#pragma once
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#include <ws2tcpip.h>
#include <winsock2.h>
#include <list>
#include "Protocol.h"
#include "RingBuf.h"
#include "CPacket.h"


struct SESSION
{
	SOCKET socket;

	int userNO;
	WCHAR nickName[15];

	int roomNO;

	CPacket sendPacket;
	CPacket recvPacket;

	eUSER_STATE state;
};

struct ROOM
{
	int m_iRoom_No;					//�� ��ȣ
	int m_iRoom_length;				//�� �̸� ����
	char* m_cpRoom_name;			//�� �̸�
	int m_iEnter_player_num;		//�� �������� ��� ��
};

int networkInit();
void networkAccept(SOCKET* listenSocket);
void networkFunc();

bool recvCheck(SESSION *session);
void sendCheck(SESSION *session);

char CheckSumCheck(WORD _type, char* _payLoad, int _payLoadSize);

//�α��� ��û
bool recvLoginRequire(SESSION* session, char byCheckSum, WORD wPayloadSize);
void sendLoginResponse(SESSION* session, char result);

//�� ����Ʈ ��û
void recvRoomListRequire();
void sendRoomList();

//�� ���� ��û
void recvRoomCreateRequire();
void sendRoomCreateResonse();

//�� ���� ��û
void recvRoomEnterRequire();
void sendRoomEnterResonse(); //���ο���
void sendRoomEnterUser(); //�ٸ� �����鿡��

//ä�� ��û
void recvChatRequire();
void sendChatResonse();

//�� ����
void recvRoomExit(); 
void sendRoomExit(); //�ٸ� �����鿡��
void sendRoomDelete(); //�����ڰ� ���� ��� �� ���� �ҽ� ��ü �����鿡��
