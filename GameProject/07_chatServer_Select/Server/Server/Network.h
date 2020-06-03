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
	int m_iRoom_No;					//방 번호
	WORD m_iRoom_length;				//방 이름 길이
	WCHAR* m_cpRoom_name;			//방 이름
	char m_iEnter_player_num;		//방 접속중인 사람 수

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

	//로그인 요청
	bool recvLoginRequire(SESSION* session, char byCheckSum, WORD wPayloadSize);
	void sendLoginResponse(SESSION* session, char result);

	//방 리스트 요청
	bool recvRoomListRequire(SESSION* session, char byCheckSum, WORD wPayloadSize);
	void sendRoomList(SESSION* session);

	//방 생성 요청
	bool recvRoomCreateRequire(SESSION* session, char byCheckSum, WORD wPayloadSize);
	void sendRoomCreateResonse(SESSION* session, ROOM* room, char result);

	//방 입장 요청
	bool recvRoomEnterRequire(SESSION* session, char byCheckSum, WORD wPayloadSize);
	void sendRoomEnterResonse(SESSION* session, ROOM* room, char result); //본인에게
	void sendRoomEnterUser(SESSION * session,ROOM* room); //다른 유저들에게

	//채팅 요청
	bool recvChatRequire(SESSION* session, char byCheckSum, WORD wPayloadSize);
	void sendChatResonse(SESSION* session, WCHAR* message, WORD messageSize);

	//방 퇴장
	bool recvRoomExit(SESSION* session, char byCheckSum, WORD wPayloadSize);
	void sendRoomExit(SESSION* session); //다른 유저들에게
	void sendRoomDelete(ROOM* room); //참가자가 없는 경우 방 삭제 소식 전체 유저들에게

	void disconnect(SESSION *session);
};