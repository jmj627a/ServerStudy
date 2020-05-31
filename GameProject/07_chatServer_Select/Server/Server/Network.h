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
	int m_iRoom_length;				//방 이름 길이
	char* m_cpRoom_name;			//방 이름
	int m_iEnter_player_num;		//방 접속중인 사람 수
};

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
void recvRoomListRequire();
void sendRoomList();

//방 생성 요청
void recvRoomCreateRequire();
void sendRoomCreateResonse();

//방 입장 요청
void recvRoomEnterRequire();
void sendRoomEnterResonse(); //본인에게
void sendRoomEnterUser(); //다른 유저들에게

//채팅 요청
void recvChatRequire();
void sendChatResonse();

//방 퇴장
void recvRoomExit(); 
void sendRoomExit(); //다른 유저들에게
void sendRoomDelete(); //참가자가 없는 경우 방 삭제 소식 전체 유저들에게
