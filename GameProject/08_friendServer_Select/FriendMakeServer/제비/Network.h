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

	//��Ŷ Ÿ�� �ְ� ��Ŷ ������ִ� �Լ�
	void make_packet(SESSION* session, CPacket* temp, int packet_type, int multimapCount = 0);

	SESSION* findAccountNo(UINT64 accountNo, int type);

	//accountNo ������ Nickname ã�Ƽ� ���� (unordered map)
	WCHAR* findNickname(UINT64 accountNo, int type);

	//�̹� ���� ������ �Ǿ��ִ°�?
	bool findRelation(UINT64 from, UINT64 to, int type);
	bool deleteRelation(UINT64 from, UINT64 to, int type);

	//ȸ�� ���� ��û
	bool recv_AccountAdd_Require(SESSION* session, WORD wPayloadSize);
	bool send_AccountAdd_Response(SESSION* session);

	//ȸ�� �α���
	bool recv_Login_Require(SESSION* session, WORD wPayloadSize);
	bool send_Login_Response(SESSION* session, WCHAR* findName);

	//ȸ�� ����Ʈ ��û
	bool recv_AccountList_Require(SESSION* session, WORD wPayloadSize);
	bool send_AccoiuntList_Response(SESSION* session);

	//ģ�� ��� ��û
	bool recv_FriendList_Require(SESSION* session, WORD wPayloadSize);
	bool send_FriendList_Response(SESSION* session);

	//ģ����û ���� ��� ��û
	bool recv_FriendRequestList_Require(SESSION* session, WORD wPayloadSize);
	bool send_FriendRequestList_Response(SESSION* session);

	//ģ�� ��û ������ ��� ��û
	bool recv_FriendReplyList_Require(SESSION* session, WORD wPayloadSize);
	bool send_FriendReplyList_Response(SESSION* session);

	//ģ�� ���� ���� 
	bool recv_FriendRemove_Require(SESSION* session, WORD wPayloadSize);
	bool send_FriendRemove_Response(SESSION* session, UINT64 requestAccountNos);

	//ģ�� ��û
	bool recv_FriendRequset_Require(SESSION* session, WORD wPayloadSize);
	bool send_FriendRequset_Response(SESSION* session, UINT64 requestAccountNo);

	//ģ�� ��û ���
	bool recv_FriendCancel_Require(SESSION* session, WORD wPayloadSize);
	bool send_FriendCancel_Response(SESSION* session, UINT64 requestAccountNo);

	//ģ�� ��û �ź�
	bool recv_FriendRequestDeny_Require(SESSION* session, WORD wPayloadSize);
	bool send_FriendRequestDeny_Response(SESSION* session, UINT64 requestAccountNo);

	//ģ�� ��û ����
	bool recv_FriendRequestAgree_Require(SESSION* session, WORD wPayloadSize);
	bool send_FriendRequestAgree_Response(SESSION* session, UINT64 requestAccountNos);

	//��Ʈ���� �׽�Ʈ�� ����
	bool recv_StressTest_Require(SESSION* session, WORD wPayloadSize);
	bool send_StressTest_Response(SESSION* session, WORD size, char* str);


	void printText();

	//���� ����
	void disconnect(SESSION *session);
};