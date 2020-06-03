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
	WORD m_iRoom_length;				//�� �̸� ����
	WCHAR* m_cpRoom_name;			//�� �̸�
	char m_iEnter_player_num;		//�� �������� ��� ��

	//std::list<int> enterPlayerNO;
};

class CNetwork
{
public:

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
	bool recvRoomListRequire(SESSION* session, char byCheckSum, WORD wPayloadSize);
	void sendRoomList(SESSION* session);

	//�� ���� ��û
	bool recvRoomCreateRequire(SESSION* session, char byCheckSum, WORD wPayloadSize);
	void sendRoomCreateResonse(SESSION* session, ROOM* room, char result);

	//�� ���� ��û
	bool recvRoomEnterRequire(SESSION* session, char byCheckSum, WORD wPayloadSize);
	void sendRoomEnterResonse(SESSION* session, ROOM* room, char result); //���ο���
	void sendRoomEnterUser(SESSION * session,ROOM* room); //�ٸ� �����鿡��

	//ä�� ��û
	bool recvChatRequire(SESSION* session, char byCheckSum, WORD wPayloadSize);
	void sendChatResonse(SESSION* session, WCHAR* message, WORD messageSize);

	//�� ����
	bool recvRoomExit(SESSION* session, char byCheckSum, WORD wPayloadSize);
	void sendRoomExit(SESSION* session); //�ٸ� �����鿡��
	void sendRoomDelete(ROOM* room); //�����ڰ� ���� ��� �� ���� �ҽ� ��ü �����鿡��

	void disconnect(SESSION *session);
};