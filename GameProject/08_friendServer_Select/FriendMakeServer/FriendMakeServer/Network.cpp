#include "Network.h"

//전체 연결된 sessionList
list<SESSION*> sessionList;

//계정 생성된 목록 <accountNo , NickName >
unordered_map<UINT64, WCHAR*> clientInfoMap;

// 이미 만들어진 친구목록 <From, 친구 구조체>
unordered_multimap<UINT64, FRIEND *> friendMap;

// 친구요청 <From, 친구 요청 구조체>
unordered_multimap<UINT64, FRIEND *> requestFriendMap;

// From 친구요청 한사람(Key), To 친구요청 받은사람(Data)
unordered_multimap<UINT64, UINT64> requestFrom;

// To 친구요청 받은사람 (Key), From 친구요청 한사람(Data)
unordered_multimap<UINT64, UINT64> requestTo;


SOCKET g_listenSocket;

UINT64 g_userNoCount = 1;
//UINT64 g_totalFriendNum = 0;

int CNetwork::networkInit()
{
	WSADATA wsaData;
	SOCKADDR_IN servAddr;

	int ret;
	// 소켓 라이브러리 초기화
	ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret != NO_ERROR) {
		wprintf(L"WSAStartup failed with error: %ld\n", ret);
		return -1;
	}

	// 소켓 생성
	g_listenSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (g_listenSocket == INVALID_SOCKET)
		return -1;

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(dfNETWORK_PORT);

	// IP 주소 PORT번호 할당
	ret = bind(g_listenSocket, (SOCKADDR*)&servAddr, sizeof(servAddr));
	if (ret == SOCKET_ERROR)
		return -1;

	// 생성한 소켓을 서버 소켓으로 완성
	ret = listen(g_listenSocket, SOMAXCONN);
	if (ret == SOCKET_ERROR)
		return -1;


	u_long on = 1;
	ioctlsocket(g_listenSocket, FIONBIO, &on);

	return 1;
}

void CNetwork::networkAccept(SOCKET* listenSocket)
{
	SOCKADDR_IN addr;
	int addrLen = sizeof(addr);

	SOCKET clientSocket = accept(*listenSocket, (SOCKADDR*)&addr, &addrLen);

	//char Ip[20];
	//inet_ntop(AF_INET, &(addr->sin_addr), Ip, INET_ADDRSTRLEN);

	SESSION* newSession = new SESSION;
	newSession->socket = clientSocket;
	newSession->userNo = 0;
	//memcpy(&newSession->ip, &addr.sin_addr, sizeof(addr.sin_addr));
	//newSession->port = addr.sin_port;

	//list에 삽입
	sessionList.push_back(newSession);

}

void CNetwork::networkFunc()
{
	FD_SET ReadSet;
	FD_SET WriteSet;

	//접속자 전체를 순회할 iter
	std::list<SESSION*>::iterator iter = sessionList.begin();
	std::list<SESSION*>::iterator startIter;

	while (true)
	{
		//계속해서 처음부터 순회
		if (iter == sessionList.end())
			iter = sessionList.begin();

		FD_ZERO(&ReadSet);
		FD_ZERO(&WriteSet);
		FD_SET(g_listenSocket, &ReadSet);

		startIter = iter;

		//전체를 순회하면서 세션 세팅.
		for (int i = 0; i < FD_SETSIZE - 1; ++i) 
		{
			if (sessionList.end() == iter)
				break;

			FD_SET((*iter)->socket, &ReadSet);

			FD_SET((*iter)->socket, &WriteSet);

			iter++;
		}

		timeval time;
		time.tv_usec = 0;
		time.tv_sec = 0;

		int ret = select(0, &ReadSet, &WriteSet, 0, &time); //지금은 timeout을 0으로 넣어야한다. 지금 여기서 잠들면 저 뒤에있는 select는 계속 기다리게됨. 대신 루프가 엄청 돌게된다.
		
		if (ret == SOCKET_ERROR)
			return;

		//listenSocket이 반응하면 accept하기
		if (FD_ISSET(g_listenSocket, &ReadSet))
		{
			networkAccept(&g_listenSocket);
		}

		//세션 리스트 돌면서 FD_ISSET으로 니가 반응한 애냐고 물어보고, 걔네 대상으로 recv 시도.
		for (int i = 0; i < FD_SETSIZE; ++i)
		{
			if (startIter == sessionList.end())
				break;

			SESSION *session = *startIter;
			++startIter;

			//recv 체크
			if (FD_ISSET(session->socket, &ReadSet))
			{
				recvCheck(session);
			}

			if (session != nullptr)
			{
				//send 체크
				if (FD_ISSET(session->socket, &WriteSet))
				{
					sendCheck(session);
				}

			if (sessionList.size() == 0)
				break;
			}
			
		}
	}
}

bool CNetwork::recvCheck(SESSION *session)
{
	//클라에 들어있는 큐 확인
	CPacket* packet = &session->recvPacket;

	//recv 
	int recvSize = recv(session->socket, packet->GetWritePtr(), packet->GetBufferSize() - packet->GetDataSize(), 0);
	packet->MoveWritePos(recvSize);

	if (recvSize <=0)
	{
		//disconnect(session);
		return false;
	}

	if (recvSize > 0)
	{
		//버퍼에서 받은거 꺼내서 버퍼 다 비워질때까지 처리.
		while (true)
		{
			if (packet->GetDataSize() < sizeof(st_PACKET_HEADER))
				return true;

			BYTE byCode;
			WORD wMsgType;
			WORD wPayloadSize;

			*packet >> byCode  >> wMsgType >> wPayloadSize;

			if (byCode != dfPACKET_CODE)
				return true;

			switch (wMsgType)
			{
				//회원가입 요청
			case df_REQ_ACCOUNT_ADD:
				recv_AccountAdd_Require(session, wPayloadSize);
				break;

				//회원 로그인
			case df_REQ_LOGIN:
				recv_Login_Require(session, wPayloadSize);
				break;

				//회원 리스트 요청
			case df_REQ_ACCOUNT_LIST:
				recv_AccountList_Require(session, wPayloadSize);
				break;

				//친구 목록 요청
			case df_REQ_FRIEND_LIST:
				recv_FriendList_Require(session, wPayloadSize);
				break;

				//친구요청 보낸 목록 요청
			case df_REQ_FRIEND_REQUEST_LIST:
				recv_FriendRequestList_Require(session, wPayloadSize);
				break;

				//친구 요청 받은거 목록 요청
			case df_REQ_FRIEND_REPLY_LIST:
				recv_FriendReplyList_Require(session, wPayloadSize);
				break;

				//친구 관계 끊기 
			case df_REQ_FRIEND_REMOVE:
				recv_FriendRemove_Require(session, wPayloadSize);
				break;

				//친구 요청
			case df_REQ_FRIEND_REQUEST:
				recv_FriendRequset_Require(session, wPayloadSize);
				break;

				//친구 요청 취소
			case df_REQ_FRIEND_CANCEL:
				recv_FriendCancel_Require(session, wPayloadSize);
				break;

				//친구 요청 거부
			case df_REQ_FRIEND_DENY:
				recv_FriendRequestDeny_Require(session, wPayloadSize);
				break;

				//친구 요청 수락
			case df_REQ_FRIEND_AGREE:
				recv_FriendRequestAgree_Require(session, wPayloadSize);
				break;

				//스트레스 테스트용 에코
			case df_REQ_STRESS_ECHO:
				break;
			}
		}
	}

	return true;
}

void CNetwork::sendCheck(SESSION *session)
{
	//클라에 들어있는 큐 확인
	CPacket* packet = &session->sendPacket;

	//send
	int sendSize = send(session->socket, packet->GetReadPtr(), packet->GetDataSize(), 0);

	if (sendSize > 0)
	{
		session->sendPacket.Clear();
		wprintf(L"[%d] send : %d byte \n", session->userNo, sendSize);
	}

	if (sendSize <=0)
		return;
}

void CNetwork::make_packet(SESSION* session, CPacket* temp, int packet_type, int multimapCount)
{
	CPacket* packet = &session->sendPacket;

	BYTE	byCode = dfPACKET_CODE;
	WORD	wMsgType = packet_type;
	WORD	wPayloadSize;
	if (packet_type == df_RES_FRIEND_LIST ||
		packet_type == df_RES_FRIEND_REQUEST_LIST ||
		packet_type == df_RES_FRIEND_REPLY_LIST)
		wPayloadSize = temp->GetDataSize() + sizeof(UINT);
	else
		wPayloadSize = temp->GetDataSize();

	*packet << byCode << wMsgType << wPayloadSize;
	if (packet_type == df_RES_FRIEND_LIST ||
		packet_type == df_RES_FRIEND_REQUEST_LIST ||
		packet_type == df_RES_FRIEND_REPLY_LIST)
	{
		*packet << (UINT)multimapCount;
		packet->PutData(temp->GetReadPtr(), wPayloadSize - sizeof(UINT));
	}
	else
		packet->PutData(temp->GetReadPtr(), wPayloadSize);
}

SESSION* CNetwork::findAccountNo(UINT64 accountNo, int type)
{
	std::list<SESSION*>::iterator startIter = sessionList.begin();
	std::list<SESSION*>::iterator endIter = sessionList.end();

	for (startIter; startIter != endIter; ++startIter)
	{
		if ((*startIter)->userNo == accountNo)
			return (*startIter);
	}

	return nullptr;
}

WCHAR* CNetwork::findNickname(UINT64 accountNo, int type)
{
	switch (type)
	{
	case MAP:
		{
			auto findIter = clientInfoMap.find(accountNo);
			if (findIter == clientInfoMap.end())
				return nullptr;
			else
				return findIter->second;
		}
		break;
	}
}

bool CNetwork::findRelation(UINT64 from, UINT64 to, int type)
{
	switch (type)
	{
		//둘이 이미 친구인가?
		case dfFRIEND:
		{
			auto iter = friendMap.find(from);
			if (iter != friendMap.end())
				return true;
			else
				return false;
		}
		break;

		//이미 친구신청을 했는가?
		case dfFRIEND_REQUEST:
		{
			auto iter = requestFrom.begin();
			pair<unordered_multimap<UINT64, UINT64>::iterator,
				unordered_multimap<UINT64, UINT64>::iterator> range;

			range = requestFrom.equal_range(from);

			for (iter = range.first; iter != range.second;)
			{
				if ((iter->first == from && iter->second == to))
				{
					return true;
				}
				else
					iter++;
			}
			
			return false;
		}
		break;
		//이미 친구신청을	 받았는가?
		case dfFRIEND_REPLY:
		{
			auto iter = requestTo.begin();
			pair<unordered_multimap<UINT64, UINT64>::iterator,
				unordered_multimap<UINT64, UINT64>::iterator> range;
			range = requestTo.equal_range(to);

			for (iter = range.first; iter != range.second;)
			{
				if ((iter->first == to && iter->second == from))
				{
					return true;
				}
				else
					iter++;
			}

			return false;
		}
		break;
	}

	return false;
}

bool CNetwork::deleteRelation(UINT64 from, UINT64 to, int type)
{
	if (!findRelation(from, to, type))
		return false;

	switch (type)
	{
		//둘이 이미 친구인가?
	case dfFRIEND:
	{
		auto iter = friendMap.begin();
		pair<unordered_multimap<UINT64, FRIEND*>::iterator,
			unordered_multimap<UINT64, FRIEND*>::iterator> range;

		range = friendMap.equal_range(from);

		for (iter = range.first; iter != range.second;)
		{
			if ((iter->second->FromAccountNo == from && iter->second->ToAccountNo == to))
			{
				iter = friendMap.erase(iter);
				break;
			}
			else
				iter++;
		}

		iter = friendMap.begin();
		range = friendMap.equal_range(to);

		for (iter = range.first; iter != range.second;)
		{
			if ((iter->second->FromAccountNo == to && iter->second->ToAccountNo == from))
			{
				iter = friendMap.erase(iter);
				return true;
			}
			else
				iter++;
		}
	}
	break;

	//이미 친구신청을 했는가?
	case dfFRIEND_REQUEST:
	{
		auto iter = requestFrom.begin();
		pair<unordered_multimap<UINT64, UINT64>::iterator,
			unordered_multimap<UINT64, UINT64>::iterator> range;

		range = requestFrom.equal_range(from);

		for (iter = range.first; iter != range.second;)
		{
			if ((iter->first == from && iter->second == to))
			{
				iter = requestFrom.erase(iter);
				break;
			}
			else
				iter++;
		}

		iter = requestTo.begin();
		range = requestTo.equal_range(to);

		for (iter = range.first; iter != range.second;)
		{
			if ((iter->first == to && iter->second == from))
			{
				iter = requestTo.erase(iter);
				return true;
			}
			else
				iter++;
		}
	}
	return false;
	}
}
bool CNetwork::recv_AccountAdd_Require(SESSION* session, WORD wPayloadSize)
{
	CPacket* packet = &session->recvPacket;

	if (packet->GetDataSize() < wPayloadSize)
		return false;

	WCHAR* nickname = new WCHAR[dfNICK_MAX_LEN];
	packet->GetData((char*)nickname, dfNICK_MAX_LEN * 2);
	
	clientInfoMap.insert({ g_userNoCount++ , nickname });

	send_AccountAdd_Response(session);
	
	wprintf(L"[%d] login succ : df_REQ_ACCOUNT_ADD \n", session->socket);
	return true;
}

bool CNetwork::send_AccountAdd_Response(SESSION* session)
{
	CPacket temp;
	temp << g_userNoCount - 1;

	make_packet(session, &temp, df_RES_ACCOUNT_ADD);
	return false;
}


bool CNetwork::recv_Login_Require(SESSION * session, WORD wPayloadSize)
{
	CPacket *packet = &session->recvPacket;
	
	if (packet->GetDataSize() < wPayloadSize)
		return false;

	UINT64 accountNo = 0;
	*packet >> accountNo;

	WCHAR* findname = findNickname(accountNo , MAP);

	if (findname != nullptr)
	{
		//있는 계정번호면 이 session에 계정번호와 닉네임을 주기
		session->userNo = accountNo;
		wcscpy(session->nickName, findname);
	}

	send_Login_Response(session, findname);
	wprintf(L"[%d] login succ : df_REQ_LOGIN \n", session->socket);

	return true;
}

bool CNetwork::send_Login_Response(SESSION * session, WCHAR* findName)
{
	CPacket temp;
	if (findName != nullptr)
	{
		temp << (UINT64)session->userNo;
		temp.PutData((char*)findName, dfNICK_MAX_LEN * 2);
	}
	else
	{
		temp << (UINT64)0;
	}
	make_packet(session, &temp, df_RES_LOGIN);

	return true;
}

bool CNetwork::recv_AccountList_Require(SESSION * session, WORD wPayloadSize)
{
	CPacket *packet = &session->recvPacket;

	if (packet->GetDataSize() < wPayloadSize)
		return false;

	send_AccoiuntList_Response(session);
	wprintf(L"[%d] ACCOUNT_LIST succ : df_REQ_ACCOUNT_LIST \n", session->socket);

	return true;
}

bool CNetwork::send_AccoiuntList_Response(SESSION * session)
{
	CPacket temp;
	
	temp << (UINT)clientInfoMap.size();

	unordered_map<UINT64, WCHAR*>::iterator iter; 
	for (iter = clientInfoMap.begin(); iter != clientInfoMap.end(); ++iter) {
		temp << (UINT64)iter->first;
		temp.PutData((char*)iter->second, dfNICK_MAX_LEN * 2);
	}

	make_packet(session, &temp, df_RES_ACCOUNT_LIST);

	return true;
}

bool CNetwork::recv_FriendList_Require(SESSION * session, WORD wPayloadSize)
{
	CPacket *packet = &session->recvPacket;

	if (packet->GetDataSize() < wPayloadSize)
		return false;

	send_FriendList_Response(session);
	wprintf(L"[%d] FRIEND_LIST succ : df_REQ_FRIEND_LIST \n", session->socket);

	return true;
}

bool CNetwork::send_FriendList_Response(SESSION * session)
{
	CPacket temp;

	UINT friendNum = 0;
	
	if (session->userNo != 0)
	{
		pair<unordered_multimap<UINT64, FRIEND*>::iterator,
			unordered_multimap<UINT64, FRIEND*>::iterator> range;

		range = friendMap.equal_range(session->userNo);

		//session->userNo를 from으로 가지는 친구 목록, 범위 순회
		for (auto iter = range.first; iter != range.second; ++iter)
		{
			friendNum++;

			//친구 계정 번호
			temp << iter->second->ToAccountNo;

			//친구 닉네임
			WCHAR *nickname = findNickname(iter->second->ToAccountNo, MAP);
			temp.PutData((char*)nickname, dfNICK_MAX_LEN * 2);
			
		}
	}

	make_packet(session, &temp, df_RES_FRIEND_LIST, friendNum);
	return true;
}

bool CNetwork::recv_FriendRequestList_Require(SESSION * session, WORD wPayloadSize)
{
	CPacket *packet = &session->recvPacket;

	if (packet->GetDataSize() < wPayloadSize)
		return false;

	send_FriendRequestList_Response(session);
	wprintf(L"[%d] FRIEND_REQUEST_LIST succ : df_REQ_FRIEND_REQUEST_LIST \n", session->socket);

	return true;
}

bool CNetwork::send_FriendRequestList_Response(SESSION * session)
{
	CPacket temp;

	UINT friendNum = 0;

	if (session->userNo != 0)
	{
		pair<unordered_multimap<UINT64, UINT64>::iterator,
			unordered_multimap<UINT64, UINT64>::iterator> range;

		range = requestFrom.equal_range(session->userNo);

		//session->userNo를 from으로 가지는 친구 목록, 범위 순회
		for (auto iter = range.first; iter != range.second; ++iter)
		{
			friendNum++;

			//친구 계정 번호
			temp << iter->second;

			//친구 닉네임
			WCHAR *nickname = findNickname(iter->second, MAP);
			temp.PutData((char*)nickname, dfNICK_MAX_LEN * 2);
			
		}
	}

	make_packet(session, &temp, df_RES_FRIEND_REQUEST_LIST, friendNum);
	return true;
}

bool CNetwork::recv_FriendReplyList_Require(SESSION * session, WORD wPayloadSize)
{
	CPacket *packet = &session->recvPacket;

	if (packet->GetDataSize() < wPayloadSize)
		return false;

	send_FriendReplyList_Response(session);
	wprintf(L"[%d] FRIEND_REPLY_LIST succ : df_REQ_FRIEND_REPLY_LIST \n", session->socket);

	return true;
}

bool CNetwork::send_FriendReplyList_Response(SESSION * session)
{
	CPacket temp;

	UINT friendNum = 0;

	if (session->userNo != 0)
	{
		pair<unordered_multimap<UINT64, UINT64>::iterator,
			unordered_multimap<UINT64, UINT64>::iterator> range;

		range = requestTo.equal_range(session->userNo);

		//session->userNo를 from으로 가지는 친구 목록, 범위 순회
		for (auto iter = range.first; iter != range.second; ++iter)
		{
			friendNum++;

			//친구 계정 번호
			temp << iter->second;

			//친구 닉네임
			WCHAR *nickname = findNickname(iter->second, MAP);
			temp.PutData((char*)nickname, dfNICK_MAX_LEN * 2);
		}
	}

	make_packet(session, &temp, df_RES_FRIEND_REPLY_LIST, friendNum);
	return true;
}

bool CNetwork::recv_FriendRemove_Require(SESSION * session, WORD wPayloadSize)
{
	CPacket *packet = &session->recvPacket;

	if (packet->GetDataSize() < wPayloadSize)
		return false;

	UINT64 requestAccountNo;

	*packet >> requestAccountNo;

	send_FriendRemove_Response(session, requestAccountNo);
	wprintf(L"[%d] FRIEND_CANCEL succ : df_REQ_FRIEND_CANCEL \n", session->socket);

	return true;
}

bool CNetwork::send_FriendRemove_Response(SESSION * session, UINT64 requestAccountNo)
{
	CPacket temp;

	if (session->userNo != 0)
	{
		//이미 친구 목록에 있는가?
		if (findRelation(session->userNo, requestAccountNo, dfFRIEND))
		{
			temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_REMOVE_OK;
			deleteRelation(requestAccountNo, session->userNo, dfFRIEND);
		}
		else
		{
			//상대 계정이 없는 계정
			if (findNickname(requestAccountNo, MAP) == nullptr)
				temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_REMOVE_FAIL;
			//내 계정을 목록에서 삭제하려고 함
			else if (requestAccountNo == session->userNo)
				temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_REMOVE_FAIL;
			//친구가 아님
			else
				temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_REMOVE_NOTFRIEND;
		}
	}
	else
		temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_CANCEL_FAIL;

	make_packet(session, &temp, df_RES_FRIEND_REMOVE);
	return true;
}

bool CNetwork::recv_FriendRequset_Require(SESSION * session, WORD wPayloadSize)
{
	CPacket *packet = &session->recvPacket;

	if (packet->GetDataSize() < wPayloadSize)
		return false;

	UINT64 requestAccountNo;

	*packet >> requestAccountNo;

	send_FriendRequset_Response(session , requestAccountNo);
	wprintf(L"[%d] FRIEND_REQUEST succ : df_REQ_FRIEND_REQUEST \n", session->socket);

	return true;
}

bool CNetwork::send_FriendRequset_Response(SESSION * session, UINT64 requestAccountNo)
{
	CPacket temp;

	if (session->userNo != 0)
	{
		//이미 친구 목록에 있는가?
		if (findRelation(session->userNo, requestAccountNo, dfFRIEND))
			temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_REQUEST_AREADY;
		else if (findRelation(session->userNo, requestAccountNo, dfFRIEND_REQUEST))
			temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_REQUEST_AREADY;
		else
		{
			if (findNickname(requestAccountNo, MAP) == nullptr)
				temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_REQUEST_NOTFOUND;
			else if (requestAccountNo == session->userNo)
				temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_REQUEST_NOTFOUND;
			else
			{
				temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_REQUEST_OK;
				requestFrom.insert({ session->userNo, requestAccountNo });
				requestTo.insert({ requestAccountNo, session->userNo });
			}
		}
	}
	else
		temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_REQUEST_NOTFOUND;

	make_packet(session, &temp, df_RES_FRIEND_REQUEST);
	return true;
}

bool CNetwork::recv_FriendCancel_Require(SESSION * session, WORD wPayloadSize)
{
	CPacket *packet = &session->recvPacket;

	if (packet->GetDataSize() < wPayloadSize)
		return false;

	UINT64 requestAccountNo;

	*packet >> requestAccountNo;

	send_FriendCancel_Response(session, requestAccountNo);
	wprintf(L"[%d] FRIEND_CANCEL succ : df_REQ_FRIEND_CANCEL \n", session->socket);

	return true;
}

bool CNetwork::send_FriendCancel_Response(SESSION * session, UINT64 requestAccountNo)
{
	CPacket temp;

	if (session->userNo != 0)
	{
		//이미 친구 신청 목록에 있는가?
		if (findRelation(session->userNo, requestAccountNo, dfFRIEND_REQUEST))
		{
			temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_CANCEL_OK;
			deleteRelation(session->userNo, requestAccountNo, dfFRIEND_REQUEST);
		}
		else
		{
			if (findNickname(requestAccountNo, MAP) == nullptr)
				temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_CANCEL_NOTFRIEND;
			else if (requestAccountNo == session->userNo)
				temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_CANCEL_NOTFRIEND;
			else
				temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_CANCEL_FAIL;
		}
	}
	else
		temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_CANCEL_FAIL;

	make_packet(session, &temp, df_RES_FRIEND_CANCEL);
	return true;
}

bool CNetwork::recv_FriendRequestDeny_Require(SESSION* session, WORD wPayloadSize)
{
	CPacket* packet = &session->recvPacket;

	if (packet->GetDataSize() < wPayloadSize)
		return false;

	UINT64 requestAccountNo;
	*packet >> requestAccountNo;
	send_FriendRequestDeny_Response(session, requestAccountNo);
	wprintf(L"[%d] FRIEND_DENY succ : df_RES_FRIEND_DENY \n", session->socket);
	return true;
}

bool CNetwork::send_FriendRequestDeny_Response(SESSION* session, UINT64 requestAccountNo)
{
	CPacket temp;

	if (session->userNo != 0)
	{
		//이미 친구 신청 목록에 있는가?
		if (findRelation(requestAccountNo, session->userNo, dfFRIEND_REQUEST))
		{
			temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_DENY_OK;
			deleteRelation(requestAccountNo, session->userNo, dfFRIEND_REQUEST);
		}
		else
		{
			if (findNickname(requestAccountNo, MAP) == nullptr)
				temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_DENY_FAIL;
			else if (requestAccountNo == session->userNo)
				temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_DENY_FAIL;
			else
				temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_DENY_NOTFRIEND;
		}
	}
	else
		temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_DENY_FAIL;

	make_packet(session, &temp, df_RES_FRIEND_DENY);
	return true;
}

bool CNetwork::recv_FriendRequestAgree_Require(SESSION * session, WORD wPayloadSize)
{
	CPacket *packet = &session->recvPacket;

	if (packet->GetDataSize() < wPayloadSize)
		return false;

	UINT64 requestAccountNo;

	*packet >> requestAccountNo;

	send_FriendRequestAgree_Response(session, requestAccountNo);
	wprintf(L"[%d] FRIEND_AGREE succ : df_REQ_FRIEND_AGREE \n", session->socket);

	return true;
}

bool CNetwork::send_FriendRequestAgree_Response(SESSION * session, UINT64 requestAccountNo)
{
	CPacket temp;

	if (session->userNo != 0)
	{
		//이미 친구 목록에 있는가?
		if(findRelation(requestAccountNo, session->userNo, dfFRIEND_REPLY))
		{
			deleteRelation(requestAccountNo, session->userNo, dfFRIEND_REQUEST);
			FRIEND* newFriend = new FRIEND(requestAccountNo, session->userNo);
			friendMap.insert({ requestAccountNo, newFriend });
			newFriend = new FRIEND(session->userNo, requestAccountNo);
			friendMap.insert({ session->userNo, newFriend});
			temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_AGREE_OK;
		}
		else
			temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_AGREE_NOTFRIEND;
	}
	else
		temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_AGREE_FAIL;

	make_packet(session, &temp, df_RES_FRIEND_AGREE);
	return true;
}


/*
void  CNetwork::disconnect(SESSION *session)
{
	// 방에 들어가 있다면
	if (session->state == eROOM)
	{
		std::list<ROOM*>::iterator iter = roomList.begin();

		for (iter; iter != roomList.end(); ++iter)
		{
			ROOM *room = (*iter);

			if (room->m_iRoom_No == session->roomNO)
			{
				//sendRoomExit(session);
				room->m_iEnter_player_num--;

				if (room->m_iEnter_player_num == 0)
				{
					// 방 삭제
					//sendRoomDelete(room);
					roomList.remove(room);
					delete room;
					break;
				}
			}
		}
	}

	sessionList.remove(session);
	closesocket(session->socket);
	delete session;
	session = nullptr;

}

//각 MsgType, Payload 의 각 바이트 더하기 % 256
//char CNetwork::CheckSumCheck(WORD _type, char* _bufPtr, int _payLoadSize)
//{
//	char checkSum;
//
//	checkSum = char(((_type) >> 8) & 0xff) + char((_type) & 0xff);
//
//	for (int i = 0; i < _payLoadSize; i++)
//		checkSum += _bufPtr[i];
//
//	checkSum %= 256;
//
//	return checkSum;
//}

*/