#include "Content.h"

//��� ���� ����� �α� ����
extern int g_iLogLevel;

//�α� ����� �ʿ��� �ӽ� ����
extern WCHAR g_szLogBuff[1024];


std::unordered_map<DWORD, st_CHARACTER*> g_CharacterMap;

extern std::list<st_CHARACTER*> g_Sector[dfSECTOR_MAX_Y][dfSECTOR_MAX_X];

void Update(void)
{
	//������ ������Ʈ ó�� Ÿ�̹� ���...
	//25fps

	
	st_CHARACTER *pCharacter = NULL;

	//ĳ���� ���� ã�Ƽ� ó��
	std::unordered_map<DWORD, st_CHARACTER*>::iterator iter = g_CharacterMap.begin();
	std::unordered_map<DWORD, st_CHARACTER*>::iterator iterEnd = g_CharacterMap.end();

	for (iter; iter != iterEnd; )
	{
		pCharacter = iter->second;
		iter++;

		//���ó��
		if (0 >= pCharacter->chHP)
		{	
			//DisconnectSession(pCharacter->socket);
		}
		else
		{
			//���� �ð����� ������ ������ ���� ó��
			DWORD dwCurrentTick = timeGetTime();
			if (dwCurrentTick - pCharacter->pSession->dwRecvTime > dfNETWORK_PACKET_RECV_TIMEOUT)
			{
				//DisconnectSession(pCharacter->pSession->socket);
				continue;
			}

			//���� ���ۿ� ���� ó��
			switch (pCharacter->dwAction)
			{
			case dfPACKET_MOVE_DIR_LL:
				pCharacter->shX = max(0, pCharacter->shX - dfSPEED_PLAYER_X);
				break;
			case dfPACKET_MOVE_DIR_LU:
				pCharacter->shX = max(0, pCharacter->shX - dfSPEED_PLAYER_X);
				pCharacter->shY = max(0, pCharacter->shY - dfSPEED_PLAYER_Y);
				break;
			case dfPACKET_MOVE_DIR_UU:
				pCharacter->shY = max(0, pCharacter->shY - dfSPEED_PLAYER_Y);
				break;
			case dfPACKET_MOVE_DIR_RU:
				pCharacter->shX = min(dfRANGE_MOVE_RIGHT, pCharacter->shX + dfSPEED_PLAYER_X);
				pCharacter->shY = max(0, pCharacter->shY - dfSPEED_PLAYER_Y);
				break;
			case dfPACKET_MOVE_DIR_RR:
				pCharacter->shX = min(dfRANGE_MOVE_RIGHT, pCharacter->shX + dfSPEED_PLAYER_X);
				break;
			case dfPACKET_MOVE_DIR_RD:
				pCharacter->shX = min(dfRANGE_MOVE_RIGHT, pCharacter->shX + dfSPEED_PLAYER_X);
				pCharacter->shY = min(dfRANGE_MOVE_BOTTOM, pCharacter->shY + dfSPEED_PLAYER_Y);
				break;
			case dfPACKET_MOVE_DIR_DD:
				pCharacter->shY = min(dfRANGE_MOVE_BOTTOM, pCharacter->shY + dfSPEED_PLAYER_Y);
				break;
			case dfPACKET_MOVE_DIR_LD:
				pCharacter->shX = max(0, pCharacter->shX - dfSPEED_PLAYER_X);
				pCharacter->shY = min(dfRANGE_MOVE_BOTTOM, pCharacter->shY + dfSPEED_PLAYER_Y);
				break;
			}



			//���� ������Ʈ ��
			if (dfPACKET_MOVE_DIR_LL <= pCharacter->dwAction &&
				pCharacter->dwAction <= dfPACKET_MOVE_DIR_LD)
			{
				//wprintf(L"before :::: player %d (%d , %d) \n", pCharacter->dwSessionID, pCharacter->shX, pCharacter->shY);
				if (Sector_UpdateCharacter(pCharacter))
				{
					//����Ǿ����� Ŭ��鿡 ���� ���
					CharacterSectorUpdatePacket(pCharacter);
					//wprintf(L"after :::: player %d (%d , %d) \n", pCharacter->dwSessionID, pCharacter->shX, pCharacter->shY);
				}
			}
		}
	}
}

st_CHARACTER * FindCharacter(DWORD _dwSessionID)
{
	auto Finditer = g_CharacterMap.find(_dwSessionID);

	if (Finditer != g_CharacterMap.end())
		return Finditer->second;

	return nullptr;
}

void CreateCharacter(st_SESSION * pSession)
{
	st_CHARACTER *character = new st_CHARACTER;
	character->socket = pSession->socket;
	character->pSession = pSession;
	character->dwSessionID = pSession->dwSessionID;

	character->dwAction = dfACTION_STAND;
	character->dwActionTick = 0;
	character->byDirection = dfACTION_MOVE_LL;
	character->byMoveDirection = dfACTION_MOVE_LL;

	character->shX = rand() % dfRANGE_MOVE_RIGHT;
	character->shY = rand() % dfRANGE_MOVE_BOTTOM;
	character->shActionX = character->shX;
	character->shActionY = character->shY;
	
	character->CurSector.iX = -1;
	character->CurSector.iY = -1;

	character->chHP = 100;

	g_CharacterMap.insert({ character->dwSessionID , character });

	//�� ĳ���� �����
	CPacket packet;
	makePacket_CreateMyCharacter(character, &packet);
	SendPacket_Unicast(pSession, &packet);
	Sector_AddCharacter(character);

	//�ٸ� Ŭ������� �� ĳ���� ���� �Ѹ���
	st_SECTOR_AROUND around;
	packet.Clear();
	makePacket_CreateOtherCharacter(character, &packet);
	GetSectorAround(character->CurSector.iX, character->CurSector.iY, &around);
	SendPacket_Around(pSession, &packet);
	
	//������ �ٸ� ĳ���� ���� �Ѹ���
	for (auto iter = g_CharacterMap.begin(); iter != g_CharacterMap.end(); ++iter)
	{
		if ((*iter).second->pSession == pSession) continue;
		makePacket_CreateOtherCharacter(iter->second, &packet);
		SendPacket_Unicast(pSession, &packet);
	}
}


// 1. ���� ���Ϳ��� ������ �κп� - ĳ���� ���� �޽���
// 2. �̵��ϴ� ĳ���Ϳ��� ���� ���Ϳ��� ���ܵ� ������ ĳ���͵� ���� ��Ű�� �޽���
// 3. ���� �߰��� ���Ϳ� - ĳ���� ���� �޽��� & �̵� �޽���
// 4. �̵��ϴ� ĳ���Ϳ��� - ���� ������ ������ ĳ���͵� ���� �޽���
void CharacterSectorUpdatePacket(st_CHARACTER * pCharacter)
{
	list<st_CHARACTER*> *pSectorList;
	list<st_CHARACTER*>::iterator listIter;

	st_SECTOR_AROUND RemoveSector, AddSector;
	CPacket packet;

	GetUpdateSectorAround(pCharacter, &RemoveSector, &AddSector);

	//1. Remove Sector�� ĳ���� ���� ��Ŷ ������
	makePacket_DeleteCharacter(pCharacter, &packet);

	for (int iCnt = 0; iCnt < RemoveSector.iCount; iCnt++)
	{
		//Ư�� ���� �� �������� �޼����� �����ϴ� �Լ�
		SendPacket_SectorOne(pCharacter->pSession, &packet, &RemoveSector.Around[iCnt]);
	}

	//2. �̵��ϴ� ĳ���Ϳ��� RemoveSector�� ĳ���͵� ���� ��Ŷ ������
	for (int iCnt = 0; iCnt < RemoveSector.iCount; iCnt++)
	{
		pSectorList = &g_Sector[RemoveSector.Around[iCnt].iY][RemoveSector.Around[iCnt].iX];

		if (pSectorList->size() == 0)
			continue;

		for (listIter = pSectorList->begin(); listIter != pSectorList->end(); ++listIter)
		{
			if ((*listIter) != pCharacter)
			{
				makePacket_DeleteCharacter((*listIter), &packet);
				SendPacket_Unicast(pCharacter->pSession, &packet);
			}
		}
	}

	//3. AddSector ���� �߰��� ���Ϳ� ĳ���� ���� �޽��� ������
	makePacket_CreateOtherCharacter(pCharacter, &packet);
	for (int iCnt = 0; iCnt < AddSector.iCount; iCnt++)
		SendPacket_SectorOne(pCharacter->pSession, &packet, &AddSector.Around[iCnt]);

	//3-1. ������ ĳ���Ϳ� �̵� ��Ŷ ������
	makePacket_MoveStart(pCharacter, &packet);
	for (int iCnt = 0; iCnt < AddSector.iCount; iCnt++)
		SendPacket_SectorOne(pCharacter->pSession, &packet, &AddSector.Around[iCnt]);


	//4. �̵��ϴ� ĳ���Ϳ��� AddSector �� �ִ� ĳ���͵� ���� �޼��� ������
	st_CHARACTER *pOtherCharacter;

	for (int iCnt = 0; iCnt < AddSector.iCount; iCnt++)
	{
		//����� ���͸� ���鼭 ���͸���Ʈ ����
		pSectorList = &g_Sector[AddSector.Around[iCnt].iY][AddSector.Around[iCnt].iX];

		if (pSectorList->size() == 0)
			continue;

		//�ش� ���͸��� ��ϵ� ĳ���͵��� �̾Ƽ� ���� ��Ŷ�� ����� ����
		for (listIter = pSectorList->begin(); listIter != pSectorList->end(); ++listIter)
		{
			pOtherCharacter = *listIter;

			//���� �ƴ� ��쿡��
			if (pOtherCharacter != pCharacter)
			{
				// �ٸ� ĳ���͵� ����
				makePacket_CreateOtherCharacter(pOtherCharacter, &packet);
				SendPacket_Unicast(pCharacter->pSession, &packet);

				//�� AddSector�� ĳ���Ͱ� �Ȱ� �ִٸ� �̵� ��Ŷ ���� ����
				switch (pOtherCharacter->dwAction)
				{
				case dfACTION_MOVE_LL:
				case dfACTION_MOVE_LU:
				case dfACTION_MOVE_UU:
				case dfACTION_MOVE_RU:
				case dfACTION_MOVE_RR:
				case dfACTION_MOVE_RD:
				case dfACTION_MOVE_DD:
				case dfACTION_MOVE_LD:
					makePacket_MoveStart(pOtherCharacter, &packet);
					SendPacket_Unicast(pCharacter->pSession, &packet);
					break;

				case dfACTION_ATTACK1:
					makePacket_Attack1(pOtherCharacter, &packet);
					SendPacket_Unicast(pCharacter->pSession, &packet);
					break;

				case dfACTION_ATTACK2:
					makePacket_Attack2(pOtherCharacter, &packet);
					SendPacket_Unicast(pCharacter->pSession, &packet);
					break;

				case dfACTION_ATTACK3:
					makePacket_Attack3(pOtherCharacter, &packet);
					SendPacket_Unicast(pCharacter->pSession, &packet);
					break;
				}
			}
		}
	}

}


bool netPacketProc_MoveStart(st_SESSION * pSession, CPacket * pPacket)
{
	BYTE byDirection;
	short shX, shY;

	*pPacket >> byDirection >> shX >> shY;

	//_LOG(dfLOG_LEVEL_DEBUG, L"DEBUG# MOVE_START #\tsessionID : %d / Direction : %d / X : %d / Y : %d !!!",
	//	pSession->dwSessionID, byDirection, shX, shY);


	//id�� ĳ���͸� �˻��Ѵ�
	st_CHARACTER *pCharacter = FindCharacter(pSession->dwSessionID);

	if (pCharacter == NULL)
	{
		_LOG(dfLOG_LEVEL_ERROR, L"ERROR# MOVE_START #\tSessionID : %d Character Not Found!!!", pSession->dwSessionID);
		return false;
	}

	//������ ��ġ�� ���� ��Ŷ�� ��ġ���� �ʹ� ū ���̰� ���ٸ� ��ũ ��Ŷ�� ������ ��ǥ ����
	if (abs(pCharacter->shX - shX) > dfERROR_RANGE || abs(pCharacter->shY - shY) > dfERROR_RANGE)
	{
		makePacket_Sync(pCharacter, pPacket);
		SendPacket_Around(pCharacter->pSession, pPacket);
	
		shX = pCharacter->shX;
		shY = pCharacter->shY;
	}

	//������ ����. ���� ��ȣ�� ���Ⱚ�� ����. 
	pCharacter->dwAction = byDirection;

	//�ܼ� ����ǥ�ÿ�. byDirection (LL RR)��, �̵��� 8����� MoveDirection�� ����
	pCharacter->byMoveDirection = byDirection;

	//������ ����
	switch (byDirection)
	{
	case dfPACKET_MOVE_DIR_RR:
	case dfPACKET_MOVE_DIR_RU:
	case dfPACKET_MOVE_DIR_RD:
		pCharacter->byDirection = dfPACKET_MOVE_DIR_RR;
		break;
	case dfPACKET_MOVE_DIR_LL:
	case dfPACKET_MOVE_DIR_LU:
	case dfPACKET_MOVE_DIR_LD:
		pCharacter->byDirection = dfPACKET_MOVE_DIR_LL;
		break;
	}

	pCharacter->shX = shX;
	pCharacter->shY = shY;


	//������ �ϸ鼭 ��ǥ�� �ణ ������ ��� ���� ������Ʈ
	if (Sector_UpdateCharacter(pCharacter))
	{
		//���Ͱ� ����� ��쿡�� Ŭ�󿡰� ���� ������ ���
		CharacterSectorUpdatePacket(pCharacter);
	}

	makePacket_MoveStart(pCharacter, pPacket);

	//�������� ����ڿ��� ��� ��Ŷ�� �Ѹ���.
	SendPacket_Around(pSession, pPacket);
	//SendPacket_Broadcast(pSession, pPacket); //���� �ȸ�������ϱ� �����׸� 

	return true;
}

bool netPacketProc_MoveStop(st_SESSION * pSession, CPacket * pPacket)
{
	BYTE byDirection;
	short shX, shY;

	*pPacket >> byDirection >> shX >> shY;

	//_LOG(dfLOG_LEVEL_DEBUG, L"DEBUG# MOVE_STOP #\tsessionID : %d / Direction : %d / X : %d / Y : %d !!!",
	//	pSession->dwSessionID, byDirection, shX, shY);

	//id�� ĳ���͸� �˻��Ѵ�
	st_CHARACTER *pCharacter = FindCharacter(pSession->dwSessionID);

	if (pCharacter == NULL)
	{
		_LOG(dfLOG_LEVEL_ERROR, L"ERROR# MOVE_STOP #\tSessionID : %d Character Not Found!!!", pSession->dwSessionID);
		return false;
	}

	//������ ��ġ�� ���� ��Ŷ�� ��ġ���� �ʹ� ū ���̰� ���ٸ� ��ũ ��Ŷ�� ������ ��ǥ ����
	if (abs(pCharacter->shX - shX) > dfERROR_RANGE || abs(pCharacter->shY - shY) > dfERROR_RANGE)
	{
		makePacket_Sync(pCharacter, pPacket);
		SendPacket_Around(pCharacter->pSession, pPacket);
	
		shX = pCharacter->shX;
		shY = pCharacter->shY;
	}

	//������ ����. ���� ��ȣ�� ���Ⱚ�� ����. 
	pCharacter->dwAction = dfACTION_STAND;

	//�ܼ� ����ǥ�ÿ�. byDirection (LL RR)��, �̵��� 8����� MoveDirection�� ����
	pCharacter->byMoveDirection = byDirection;

	pCharacter->shX = shX;
	pCharacter->shY = shY;


	//������ �ϸ鼭 ��ǥ�� �ణ ������ ��� ���� ������Ʈ
	if (Sector_UpdateCharacter(pCharacter))
	{
		//���Ͱ� ����� ��쿡�� Ŭ�󿡰� ���� ������ ���
		CharacterSectorUpdatePacket(pCharacter);
	}

	makePacket_MoveStop(pCharacter, pPacket);

	//�������� ����ڿ��� ��� ��Ŷ�� �Ѹ���.
	SendPacket_Around(pSession, pPacket);
	//SendPacket_Broadcast(pSession, pPacket); //���� �ȸ�������ϱ� �����׸� 

	return true;
}

bool netPacketProc_Attack1(st_SESSION * pSession, CPacket * pPacket)
{
	BYTE byDirection;
	short shX, shY;

	*pPacket >> byDirection >> shX >> shY;

	//_LOG(dfLOG_LEVEL_DEBUG, L"DEBUG# ATTACK1 #\tsessionID : %d / Direction : %d / X : %d / Y : %d ",
	//	pSession->dwSessionID, byDirection, shX, shY);

	//id�� ĳ���͸� �˻��Ѵ�
	st_CHARACTER *pCharacter = FindCharacter(pSession->dwSessionID);

	if (pCharacter == NULL)
	{
		_LOG(dfLOG_LEVEL_ERROR, L"ERROR# ATTACK1 #\tSessionID : %d Character Not Found!!!", pSession->dwSessionID);
		return false;
	}

	//������ ��ġ�� ���� ��Ŷ�� ��ġ���� �ʹ� ū ���̰� ���ٸ� ��ũ ��Ŷ�� ������ ��ǥ ����
	if (abs(pCharacter->shX - shX) > dfERROR_RANGE || abs(pCharacter->shY - shY) > dfERROR_RANGE)
	{
		makePacket_Sync(pCharacter, pPacket);
		SendPacket_Around(pCharacter->pSession, pPacket);
	
		shX = pCharacter->shX;
		shY = pCharacter->shY;
	}

	//������ ����. ���� ��ȣ�� ���Ⱚ�� ����. 
	pCharacter->dwAction = dfACTION_ATTACK1;

	//�ܼ� ����ǥ�ÿ�. byDirection (LL RR)��, �̵��� 8����� MoveDirection�� ����
	pCharacter->byMoveDirection = byDirection;

	pCharacter->shX = shX;
	pCharacter->shY = shY;


	//������ �ϸ鼭 ��ǥ�� �ణ ������ ��� ���� ������Ʈ
	if (Sector_UpdateCharacter(pCharacter))
	{
		//���Ͱ� ����� ��쿡�� Ŭ�󿡰� ���� ������ ���
		CharacterSectorUpdatePacket(pCharacter);
	}

	makePacket_Attack1(pCharacter, pPacket);

	//�������� ����ڿ��� ��� ��Ŷ�� �Ѹ���.
	SendPacket_Around(pSession, pPacket);


	st_SECTOR_AROUND stAroundSector;
	GetSectorAround(pCharacter->CurSector.iX, pCharacter->CurSector.iY, &stAroundSector);

	for (int iCnt = 0; iCnt < stAroundSector.iCount; iCnt++)
	{
		auto iter = g_Sector[stAroundSector.Around[iCnt].iY][stAroundSector.Around[iCnt].iX].begin();
		auto endIter = g_Sector[stAroundSector.Around[iCnt].iY][stAroundSector.Around[iCnt].iX].end();
		
		for (iter; iter != endIter; ++iter)
		{
			st_CHARACTER *pOtherCharacter = (*iter);

			if (pCharacter != pOtherCharacter)
			{
				if (pCharacter->byDirection == dfPACKET_MOVE_DIR_LL)
				{
					if (pCharacter->shX < pOtherCharacter->shX)
						continue;
					if (abs(pCharacter->shX - pOtherCharacter->shX )< dfATTACK1_RANGE_X &&
						abs(pCharacter->shY - pOtherCharacter->shY )< dfATTACK1_RANGE_Y)
					{
						pOtherCharacter->chHP -= dfATTACK1_DAMAGE;

						makePacket_Damage(pCharacter, pOtherCharacter, pPacket);
						SendPacket_Around(pCharacter->pSession, pPacket, true);
					}
				}
				else if (pCharacter->byDirection == dfPACKET_MOVE_DIR_RR)
				{
					if (pOtherCharacter->shX < pCharacter->shX)
						continue;
					if (abs(pOtherCharacter->shX - pCharacter->shX ) < dfATTACK1_RANGE_X &&
						abs(pOtherCharacter->shY - pCharacter->shY ) < dfATTACK1_RANGE_Y)
					{
						pOtherCharacter->chHP -= dfATTACK1_DAMAGE;

						makePacket_Damage(pCharacter, pOtherCharacter, pPacket);
						SendPacket_Around(pCharacter->pSession, pPacket, true);

					}
				}
			}
		}
	}
	return true;
}

bool netPacketProc_Attack2(st_SESSION * pSession, CPacket * pPacket)
{
	BYTE byDirection;
	short shX, shY;

	*pPacket >> byDirection >> shX >> shY;

	//_LOG(dfLOG_LEVEL_DEBUG, L"DEBUG# ATTACK2 #\tsessionID : %d / Direction : %d / X : %d / Y : %d ",
	//	pSession->dwSessionID, byDirection, shX, shY);

	//id�� ĳ���͸� �˻��Ѵ�
	st_CHARACTER *pCharacter = FindCharacter(pSession->dwSessionID);

	if (pCharacter == NULL)
	{
		_LOG(dfLOG_LEVEL_ERROR, L"ERROR# ATTACK2 #\tSessionID : %d Character Not Found!!!", pSession->dwSessionID);
		return false;
	}

	//������ ��ġ�� ���� ��Ŷ�� ��ġ���� �ʹ� ū ���̰� ���ٸ� ��ũ ��Ŷ�� ������ ��ǥ ����
	if (abs(pCharacter->shX - shX) > dfERROR_RANGE || abs(pCharacter->shY - shY) > dfERROR_RANGE)
	{
		makePacket_Sync(pCharacter, pPacket);
		SendPacket_Around(pCharacter->pSession, pPacket);
	
		shX = pCharacter->shX;
		shY = pCharacter->shY;
	}

	//������ ����. ���� ��ȣ�� ���Ⱚ�� ����. 
	pCharacter->dwAction = dfACTION_ATTACK2;

	//�ܼ� ����ǥ�ÿ�. byDirection (LL RR)��, �̵��� 8����� MoveDirection�� ����
	pCharacter->byMoveDirection = byDirection;

	pCharacter->shX = shX;
	pCharacter->shY = shY;


	//������ �ϸ鼭 ��ǥ�� �ణ ������ ��� ���� ������Ʈ
	if (Sector_UpdateCharacter(pCharacter))
	{
		//���Ͱ� ����� ��쿡�� Ŭ�󿡰� ���� ������ ���
		CharacterSectorUpdatePacket(pCharacter);
	}

	makePacket_Attack2(pCharacter, pPacket);

	//�������� ����ڿ��� ��� ��Ŷ�� �Ѹ���.
	SendPacket_Around(pSession, pPacket);
	//SendPacket_Broadcast(pSession, pPacket); //���� �ȸ�������ϱ� �����׸� 
	
	st_SECTOR_AROUND stAroundSector;
	GetSectorAround(pCharacter->CurSector.iX, pCharacter->CurSector.iY, &stAroundSector);

	for (int iCnt = 0; iCnt < stAroundSector.iCount; iCnt++)
	{
		auto iter = g_Sector[stAroundSector.Around[iCnt].iY][stAroundSector.Around[iCnt].iX].begin();
		auto endIter = g_Sector[stAroundSector.Around[iCnt].iY][stAroundSector.Around[iCnt].iX].end();

		for (iter; iter != endIter; ++iter)
		{
			st_CHARACTER *pOtherCharacter = (*iter);

			if (pCharacter != pOtherCharacter)
			{
				if (pCharacter->byDirection == dfPACKET_MOVE_DIR_LL)
				{
					if (pCharacter->shX < pOtherCharacter->shX)
						continue;
					if (abs(pCharacter->shX - pOtherCharacter->shX) < dfATTACK2_RANGE_X &&
						abs(pCharacter->shY - pOtherCharacter->shY) < dfATTACK2_RANGE_Y)
					{
						pOtherCharacter->chHP -= dfATTACK2_DAMAGE;

						makePacket_Damage(pCharacter, pOtherCharacter, pPacket);
						SendPacket_Around(pCharacter->pSession, pPacket, true);
					}
				}
				else if (pCharacter->byDirection == dfPACKET_MOVE_DIR_RR)
				{
					if (pOtherCharacter->shX < pCharacter->shX)
						continue;
					if (abs(pOtherCharacter->shX - pCharacter->shX) < dfATTACK2_RANGE_X &&
						abs(pOtherCharacter->shY - pCharacter->shY) < dfATTACK2_RANGE_Y)
					{
						pOtherCharacter->chHP -= dfATTACK2_DAMAGE;

						makePacket_Damage(pCharacter, pOtherCharacter, pPacket);
						SendPacket_Around(pCharacter->pSession, pPacket, true);

					}
				}
			}
		}
	}

	return true;
}

bool netPacketProc_Attack3(st_SESSION * pSession, CPacket * pPacket)
{
	BYTE byDirection;
	short shX, shY;

	*pPacket >> byDirection >> shX >> shY;

	//_LOG(dfLOG_LEVEL_DEBUG, L"DEBUG# ATTACK3 #\tsessionID : %d / Direction : %d / X : %d / Y : %d ",
	//	pSession->dwSessionID, byDirection, shX, shY);

	//id�� ĳ���͸� �˻��Ѵ�
	st_CHARACTER *pCharacter = FindCharacter(pSession->dwSessionID);

	if (pCharacter == NULL)
	{
		_LOG(dfLOG_LEVEL_ERROR, L"ERROR# ATTACK3 #\tSessionID : %d Character Not Found!!!", pSession->dwSessionID);
		return false;
	}

	//������ ��ġ�� ���� ��Ŷ�� ��ġ���� �ʹ� ū ���̰� ���ٸ� ��ũ ��Ŷ�� ������ ��ǥ ����
	if (abs(pCharacter->shX - shX) > dfERROR_RANGE || abs(pCharacter->shY - shY) > dfERROR_RANGE)
	{
		makePacket_Sync(pCharacter, pPacket);
		SendPacket_Around(pCharacter->pSession, pPacket);
	
		shX = pCharacter->shX;
		shY = pCharacter->shY;
	}

	//������ ����. ���� ��ȣ�� ���Ⱚ�� ����. 
	pCharacter->dwAction = dfACTION_ATTACK3;

	//�ܼ� ����ǥ�ÿ�. byDirection (LL RR)��, �̵��� 8����� MoveDirection�� ����
	pCharacter->byMoveDirection = byDirection;

	pCharacter->shX = shX;
	pCharacter->shY = shY;


	//������ �ϸ鼭 ��ǥ�� �ణ ������ ��� ���� ������Ʈ
	if (Sector_UpdateCharacter(pCharacter))
	{
		//���Ͱ� ����� ��쿡�� Ŭ�󿡰� ���� ������ ���
		CharacterSectorUpdatePacket(pCharacter);
	}

	makePacket_Attack3(pCharacter, pPacket);

	//�������� ����ڿ��� ��� ��Ŷ�� �Ѹ���.
	SendPacket_Around(pSession, pPacket);
	//SendPacket_Broadcast(pSession, pPacket); //���� �ȸ�������ϱ� �����׸� 

	st_SECTOR_AROUND stAroundSector;
	GetSectorAround(pCharacter->CurSector.iX, pCharacter->CurSector.iY, &stAroundSector);

	for (int iCnt = 0; iCnt < stAroundSector.iCount; iCnt++)
	{
		auto iter = g_Sector[stAroundSector.Around[iCnt].iY][stAroundSector.Around[iCnt].iX].begin();
		auto endIter = g_Sector[stAroundSector.Around[iCnt].iY][stAroundSector.Around[iCnt].iX].end();

		for (iter; iter != endIter; ++iter)
		{
			st_CHARACTER *pOtherCharacter = (*iter);

			if (pCharacter != pOtherCharacter)
			{
				if (pCharacter->byDirection == dfPACKET_MOVE_DIR_LL)
				{
					if (pCharacter->shX < pOtherCharacter->shX)
						continue;
					if (abs(pCharacter->shX - pOtherCharacter->shX) < dfATTACK3_RANGE_X &&
						abs(pCharacter->shY - pOtherCharacter->shY) < dfATTACK3_RANGE_Y)
					{
						pOtherCharacter->chHP -= dfATTACK3_DAMAGE;

						makePacket_Damage(pCharacter, pOtherCharacter, pPacket);
						SendPacket_Around(pCharacter->pSession, pPacket, true);
					}
				}
				else if (pCharacter->byDirection == dfPACKET_MOVE_DIR_RR)
				{
					if (pOtherCharacter->shX < pCharacter->shX)
						continue;
					if (abs(pOtherCharacter->shX - pCharacter->shX) < dfATTACK3_RANGE_X &&
						abs(pOtherCharacter->shY - pCharacter->shY) < dfATTACK3_RANGE_Y)
					{
						pOtherCharacter->chHP -= dfATTACK3_DAMAGE;

						makePacket_Damage(pCharacter, pOtherCharacter, pPacket);
						SendPacket_Around(pCharacter->pSession, pPacket, true);

					}
				}
			}
		}
	}

	return true;
}

bool netPacketProc_ECHO(st_SESSION * pSession, CPacket * pPacket)
{
	st_CHARACTER *pCharacter = FindCharacter(pSession->dwSessionID);
	makePacket_ECHO(pCharacter, pPacket);
	SendPacket_Unicast(pSession, pPacket);
	return false;
}

void makePacket_CreateMyCharacter(st_CHARACTER * pCharacter, CPacket * pPacket)
{
	CPacket temp;
	temp << (DWORD)pCharacter->dwSessionID;
	temp << (BYTE)pCharacter->byMoveDirection;
	temp << (WORD)pCharacter->shX;
	temp << (WORD)pCharacter->shY;
	temp << (BYTE)pCharacter->chHP;

	st_PACKET_HEADER Header;
	Header.byCode = dfPACKET_CODE;
	Header.bySize = (BYTE)temp.GetDataSize();
	Header.byType = dfPACKET_SC_CREATE_MY_CHARACTER;

	pPacket->Clear();
	pPacket->PutData((char*)(&Header), sizeof(st_PACKET_HEADER));
	pPacket->PutData(temp.GetReadPtr(), Header.bySize);

	*pPacket << (BYTE)dfNETWORK_PACKET_END;
}

void makePacket_CreateOtherCharacter(st_CHARACTER * pCharacter, CPacket * pPacket)
{
	CPacket temp;
	temp << (DWORD)pCharacter->dwSessionID;
	temp << (BYTE)pCharacter->byMoveDirection;
	temp << (WORD)pCharacter->shX;
	temp << (WORD)pCharacter->shY;
	temp << (BYTE)pCharacter->chHP;

	st_PACKET_HEADER Header;
	Header.byCode = dfPACKET_CODE;
	Header.bySize = (BYTE)temp.GetDataSize();
	Header.byType = dfPACKET_SC_CREATE_OTHER_CHARACTER;

	pPacket->Clear();
	pPacket->PutData((char*)(&Header), sizeof(st_PACKET_HEADER));
	pPacket->PutData(temp.GetReadPtr(), Header.bySize);

	*pPacket << (BYTE)dfNETWORK_PACKET_END;
}

void makePacket_DeleteCharacter(st_CHARACTER * pCharacter, CPacket * pPacket)
{
	CPacket temp;
	temp << (DWORD)pCharacter->dwSessionID;

	st_PACKET_HEADER Header;
	Header.byCode = dfPACKET_CODE;
	Header.bySize = (BYTE)temp.GetDataSize();
	Header.byType = (BYTE)dfPACKET_SC_DELETE_CHARACTER;

	pPacket->Clear();
	pPacket->PutData((char*)(&Header), sizeof(st_PACKET_HEADER));
	pPacket->PutData(temp.GetReadPtr(), Header.bySize);

	*pPacket << (BYTE)dfNETWORK_PACKET_END;
}

void makePacket_MoveStart(st_CHARACTER* pCharacter, CPacket* pPacket)
{
	CPacket temp;
	temp << (DWORD)pCharacter->dwSessionID;
	temp << (BYTE)pCharacter->byMoveDirection;
	temp << (WORD)pCharacter->shX;
	temp << (WORD)pCharacter->shY;

	st_PACKET_HEADER Header;
	Header.byCode = dfPACKET_CODE;
	Header.bySize = (BYTE)temp.GetDataSize();
	Header.byType = dfPACKET_SC_MOVE_START;

	pPacket->Clear();
	pPacket->PutData((char*)(&Header), sizeof(st_PACKET_HEADER));
	pPacket->PutData(temp.GetReadPtr(), Header.bySize);

	*pPacket << (BYTE)dfNETWORK_PACKET_END;
}

void makePacket_MoveStop(st_CHARACTER * pCharacter, CPacket * pPacket)
{
	CPacket temp;
	temp << (DWORD)pCharacter->dwSessionID;
	temp << (BYTE)pCharacter->byMoveDirection;
	temp << (WORD)pCharacter->shX;
	temp << (WORD)pCharacter->shY;

	st_PACKET_HEADER Header;
	Header.byCode = dfPACKET_CODE;
	Header.bySize = (BYTE)temp.GetDataSize();
	Header.byType = dfPACKET_SC_MOVE_STOP;

	pPacket->Clear();
	pPacket->PutData((char*)(&Header), sizeof(st_PACKET_HEADER));
	pPacket->PutData(temp.GetReadPtr(), Header.bySize);

	*pPacket << (BYTE)dfNETWORK_PACKET_END;
}

void makePacket_Attack1(st_CHARACTER * pCharacter, CPacket * pPacket)
{
	CPacket temp;
	temp << (DWORD)pCharacter->dwSessionID;
	temp << (BYTE)pCharacter->byDirection;
	temp << (WORD)pCharacter->shX;
	temp << (WORD)pCharacter->shY;

	st_PACKET_HEADER Header;
	Header.byCode = dfPACKET_CODE;
	Header.bySize = (BYTE)temp.GetDataSize();
	Header.byType = dfPACKET_SC_ATTACK1;

	pPacket->Clear();
	pPacket->PutData((char*)(&Header), sizeof(st_PACKET_HEADER));
	pPacket->PutData(temp.GetReadPtr(), Header.bySize);

	*pPacket << (BYTE)dfNETWORK_PACKET_END;
}

void makePacket_Attack2(st_CHARACTER * pCharacter, CPacket * pPacket)
{
	CPacket temp;
	temp << (DWORD)pCharacter->dwSessionID;
	temp << (BYTE)pCharacter->byDirection;
	temp << (WORD)pCharacter->shX;
	temp << (WORD)pCharacter->shY;

	st_PACKET_HEADER Header;
	Header.byCode = dfPACKET_CODE;
	Header.bySize = (BYTE)temp.GetDataSize();
	Header.byType = dfPACKET_SC_ATTACK2;

	pPacket->Clear();
	pPacket->PutData((char*)(&Header), sizeof(st_PACKET_HEADER));
	pPacket->PutData(temp.GetReadPtr(), Header.bySize);

	*pPacket << (BYTE)dfNETWORK_PACKET_END;
}

void makePacket_Attack3(st_CHARACTER * pCharacter, CPacket * pPacket)
{
	CPacket temp;
	temp << (DWORD)pCharacter->dwSessionID;
	temp << (BYTE)pCharacter->byDirection;
	temp << (WORD)pCharacter->shX;
	temp << (WORD)pCharacter->shY;

	st_PACKET_HEADER Header;
	Header.byCode = dfPACKET_CODE;
	Header.bySize = (BYTE)temp.GetDataSize();
	Header.byType = dfPACKET_SC_ATTACK3;

	pPacket->Clear();
	pPacket->PutData((char*)(&Header), sizeof(st_PACKET_HEADER));
	pPacket->PutData(temp.GetReadPtr(), Header.bySize);

	*pPacket << (BYTE)dfNETWORK_PACKET_END;
}

void makePacket_Damage(st_CHARACTER * pAttackCharacter, st_CHARACTER * pDamageCharacter, CPacket * pPacket)
{
	CPacket temp;
	temp << (DWORD)pAttackCharacter->dwSessionID;
	temp << (DWORD)pDamageCharacter->dwSessionID;
	temp << (BYTE)pDamageCharacter->chHP;

	st_PACKET_HEADER Header;
	Header.byCode = dfPACKET_CODE;
	Header.bySize = (BYTE)temp.GetDataSize();
	Header.byType = dfPACKET_SC_DAMAGE;

	pPacket->Clear();
	pPacket->PutData((char*)(&Header), sizeof(st_PACKET_HEADER));
	pPacket->PutData(temp.GetReadPtr(), Header.bySize);

	*pPacket << (BYTE)dfNETWORK_PACKET_END;
}

void makePacket_Sync(st_CHARACTER * pCharacter, CPacket * pPacket)
{
	st_PACKET_HEADER Header;

	CPacket temp;
	temp << (DWORD)pCharacter->dwSessionID;
	temp << (WORD)pCharacter->shX;
	temp << (WORD)pCharacter->shY;

	Header.byCode = dfPACKET_CODE;
	Header.bySize = temp.GetDataSize();
	Header.byType = dfPACKET_SC_SYNC;

	pPacket->Clear();
	pPacket->PutData((char*)(&Header), sizeof(st_PACKET_HEADER));
	pPacket->PutData(temp.GetReadPtr(), Header.bySize);

	*pPacket << (BYTE)dfNETWORK_PACKET_END;
}

void makePacket_ECHO(st_CHARACTER * pCharacter, CPacket * pPacket)
{
	st_PACKET_HEADER Header;

	CPacket temp;
	DWORD time = timeGetTime();
	temp << (DWORD)time;

	Header.byCode = dfPACKET_CODE;
	Header.bySize = temp.GetDataSize();
	Header.byType = dfPACKET_SC_ECHO;

	pPacket->Clear();
	pPacket->PutData((char*)(&Header), sizeof(st_PACKET_HEADER));
	pPacket->PutData(temp.GetReadPtr(), Header.bySize);

	*pPacket << (BYTE)dfNETWORK_PACKET_END;
}
