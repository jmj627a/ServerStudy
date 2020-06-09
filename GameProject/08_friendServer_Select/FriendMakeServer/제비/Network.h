#pragma once
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Winmm.lib")
#include <ws2tcpip.h>
#include <winsock2.h>
#include <list>
#include <unordered_map>
#include <iostream>
#include "Protocol.h"
#include "RingBuf.h"
#include "CPacket.h"

using namespace std;

#define LIST 0
#define MAP 1

#define dfFRIEND 0
#define dfFRIEND_REQUEST 1
#define dfFRIEND_REPLY 2

struct SESSION
{
	SOCKET socket;

	UINT64 userNo;
	WCHAR nickName[dfNICK_MAX_LEN];

	//streaming queue
	CPacket sendPacket;
	CPacket recvPacket;

	int recvFlag = 0;
	int sendFlag = 0;
};

struct FRIEND
{
	UINT64 FromAccountNo;
	UINT64 ToAccountNo;

	FRIEND(UINT64 from, UINT64 to)
	{
		FromAccountNo = from;
		ToAccountNo = to;
	}
};


class CNetwork
{
public:

	int networkInit();
	void networkAccept(SOCKET* listenSocket);
	void networkFunc();

	void SocketSelect(DWORD* pTableNo, SOCKET* pTableSocket, FD_SET *readSet, FD_SET *writeSet);

	bool recvCheck(SESSION *session);
	void sendCheck(SESSION *session);

	void testFunc_flagFalse();

	//char CheckSumCheck(WORD _type, char* _payLoad, int _payLoadSize);

	//패킷 타입 넣고 패킷 만들어주는 함수
	void make_packet(SESSION* session, CPacket* temp, int packet_type, int multimapCount = 0);

	SESSION* findAccountNo(UINT64 accountNo, int type);

	//accountNo 넣으면 Nickname 찾아서 리턴 (unordered map)
	WCHAR* findNickname(UINT64 accountNo, int type);

	//이미 둘이 연결이 되어있는가?
	bool findRelation(UINT64 from, UINT64 to, int type);
	bool deleteRelation(UINT64 from, UINT64 to, int type);

	//회원 가입 요청
	bool recv_AccountAdd_Require(SESSION* session, WORD wPayloadSize);
	bool send_AccountAdd_Response(SESSION* session);

	//회원 로그인
	bool recv_Login_Require(SESSION* session, WORD wPayloadSize);
	bool send_Login_Response(SESSION* session, WCHAR* findName);

	//회원 리스트 요청
	bool recv_AccountList_Require(SESSION* session, WORD wPayloadSize);
	bool send_AccoiuntList_Response(SESSION* session);

	//친구 목록 요청
	bool recv_FriendList_Require(SESSION* session, WORD wPayloadSize);
	bool send_FriendList_Response(SESSION* session);

	//친구요청 보낸 목록 요청
	bool recv_FriendRequestList_Require(SESSION* session, WORD wPayloadSize);
	bool send_FriendRequestList_Response(SESSION* session);

	//친구 요청 받은거 목록 요청
	bool recv_FriendReplyList_Require(SESSION* session, WORD wPayloadSize);
	bool send_FriendReplyList_Response(SESSION* session);

	//친구 관계 끊기 
	bool recv_FriendRemove_Require(SESSION* session, WORD wPayloadSize);
	bool send_FriendRemove_Response(SESSION* session, UINT64 requestAccountNos);

	//친구 요청
	bool recv_FriendRequset_Require(SESSION* session, WORD wPayloadSize);
	bool send_FriendRequset_Response(SESSION* session, UINT64 requestAccountNo);

	//친구 요청 취소
	bool recv_FriendCancel_Require(SESSION* session, WORD wPayloadSize);
	bool send_FriendCancel_Response(SESSION* session, UINT64 requestAccountNo);

	//친구 요청 거부
	bool recv_FriendRequestDeny_Require(SESSION* session, WORD wPayloadSize);
	bool send_FriendRequestDeny_Response(SESSION* session, UINT64 requestAccountNo);

	//친구 요청 수락
	bool recv_FriendRequestAgree_Require(SESSION* session, WORD wPayloadSize);
	bool send_FriendRequestAgree_Response(SESSION* session, UINT64 requestAccountNos);

	//스트레스 테스트용 에코
	bool recv_StressTest_Require(SESSION* session, WORD wPayloadSize);
	bool send_StressTest_Response(SESSION* session, WORD size, char* str);


	void printText();

	//연결 끊김
	void disconnect(SESSION *session);
};