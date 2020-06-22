#include "Sector.h"

std::list<st_CHARACTER*> g_Sector[dfSECTOR_MAX_Y][dfSECTOR_MAX_X];
extern int g_iLogLevel;
extern WCHAR g_szLogBuff[1024];

void Sector_AddCharacter(st_CHARACTER * pCharacter)
{
	if (pCharacter == nullptr)
		return;

	if (pCharacter->CurSector.iX != -1 || pCharacter->CurSector.iY != -1)
		return;

	int iSectorX = pCharacter->shX / dfSECTOR_WIDTH;
	int iSectorY = pCharacter->shY / dfSECTOR_HEIGHT;

	if (iSectorX >= dfSECTOR_MAX_X || iSectorY >= dfSECTOR_MAX_Y)
		return;


 	g_Sector[iSectorY][iSectorX].push_back(pCharacter);
	
	pCharacter->OldSector.iX = pCharacter->CurSector.iX;
	pCharacter->OldSector.iY = pCharacter->CurSector.iY;

	pCharacter->CurSector.iX = iSectorX;
	pCharacter->CurSector.iY = iSectorY;

	_LOG(dfLOG_LEVEL_DEBUG, L"DEBUG# Player %d (%d,%d) insert in [%d][%d] +",
		pCharacter->dwSessionID, pCharacter->shX, pCharacter->shY, pCharacter->CurSector.iY, pCharacter->CurSector.iX);
}

void Sector_RemoveCharacter(st_CHARACTER * pCharacter)
{
	if (pCharacter == nullptr)
		return;

	if (pCharacter->CurSector.iX == -1 || pCharacter->CurSector.iY == -1)
		return;

	list<st_CHARACTER *> *pList = &g_Sector[pCharacter->CurSector.iY][pCharacter->CurSector.iX];
	//_OUTPUTDEBUG(L"Sector RemoveCharacter [%d %d]\n", pPlayer->CurPos.iY, pPlayer->CurPos.iX);

	auto iter = pList->begin();

	bool isSuc = false;
	while (iter != pList->end())
	{
		if (pCharacter == (*iter))
		{
			pList->erase(iter);
			isSuc = true;
			_LOG(dfLOG_LEVEL_DEBUG, L"DEBUG# Player %d remove in [%d][%d] - ", 
				pCharacter->dwSessionID, pCharacter->CurSector.iY, pCharacter->CurSector.iX);

			break;
		}
		iter++;
	}

	//if (!isSuc)
	//	_LOG(dfLOG_LEVEL_ERROR, L"Player %d isn't Exist in List", pCharacter->dwSessionID);

	pCharacter->OldSector.iX = pCharacter->CurSector.iX;
	pCharacter->OldSector.iY = pCharacter->CurSector.iY;
	pCharacter->CurSector.iX = -1;
	pCharacter->CurSector.iY = -1;
}

bool Sector_UpdateCharacter(st_CHARACTER * pCharacter)
{
	if (pCharacter == nullptr)
		return false;

	int OldSectorX = pCharacter->CurSector.iX;
	int OldSectorY = pCharacter->CurSector.iY;

	int iNewSectorX = pCharacter->shX / dfSECTOR_WIDTH;
	int iNewSectorY = pCharacter->shY / dfSECTOR_HEIGHT;

	if (iNewSectorX == OldSectorX && iNewSectorY == OldSectorY)
		return false;

	if (iNewSectorX < 0 || iNewSectorX > dfSECTOR_MAX_X || iNewSectorY < 0 || iNewSectorY > dfSECTOR_MAX_Y)
		return false;

	Sector_RemoveCharacter(pCharacter);
	Sector_AddCharacter(pCharacter);

	pCharacter->CurSector.iX = iNewSectorX;
	pCharacter->CurSector.iY = iNewSectorY;
	pCharacter->OldSector.iX = OldSectorX;
	pCharacter->OldSector.iY = OldSectorY;

	return true;
}

void GetSectorAround(int iSectorX, int iSectorY, st_SECTOR_AROUND * pSectorAround)
{
	int iCntX, iCntY;

	iSectorX--;
	iSectorY--;

	pSectorAround->iCount = 0;

	for (iCntY = 0; iCntY < 3; iCntY++)
	{
		if (iSectorY + iCntY < 0 || iSectorY + iCntY >= dfSECTOR_MAX_Y)
			continue;
		for (iCntX = 0; iCntX < 3; iCntX++)
		{
			if (iSectorX + iCntX < 0 || iSectorX + iCntX >= dfSECTOR_MAX_Y)
				continue;
			pSectorAround->Around[pSectorAround->iCount].iX = iSectorX + iCntX;
			pSectorAround->Around[pSectorAround->iCount].iY = iSectorY + iCntY;
			pSectorAround->iCount++;
		}
	}
}

void GetUpdateSectorAround(st_CHARACTER * pCharacter, st_SECTOR_AROUND * pRemoveSector, st_SECTOR_AROUND * pAddSector)
{
	if (pCharacter == nullptr)
		return;

	st_SECTOR_AROUND OldSectorAround, CurSectorAround;

	OldSectorAround.iCount = 0;
	CurSectorAround.iCount = 0;

	pRemoveSector->iCount = 0;
	pAddSector->iCount = 0;

	GetSectorAround(pCharacter->OldSector.iX, pCharacter->OldSector.iY, &OldSectorAround);
	GetSectorAround(pCharacter->CurSector.iX, pCharacter->CurSector.iY, &CurSectorAround);

	// 이전 섹터 중 신규섹터에 없는 정보를 찾은뒤에 RemoveSector에 넣는다.
	for (int iCntOld = 0; iCntOld < OldSectorAround.iCount; iCntOld++)
	{
		bool bFind = false;
		for (int iCntCur = 0; iCntCur < CurSectorAround.iCount; iCntCur++)
		{
			if (OldSectorAround.Around[iCntOld].iX == CurSectorAround.Around[iCntCur].iX &&
				OldSectorAround.Around[iCntOld].iY == CurSectorAround.Around[iCntCur].iY)
			{
				bFind = true;
				break;
			}
		}
		if (bFind == false)
		{
			pRemoveSector->Around[pRemoveSector->iCount] = OldSectorAround.Around[iCntOld];
			pRemoveSector->iCount++;
		}
	}

	// 현재 섹터정보중 이전 섹터에 없는 정보를 찾아서 AddSector에 넣는다.
	for (int iCntCur = 0; iCntCur < CurSectorAround.iCount; iCntCur++)
	{
		bool bFind = false;
		for (int iCntOld = 0; iCntOld < OldSectorAround.iCount; iCntOld++)
		{
			if (OldSectorAround.Around[iCntOld].iX == CurSectorAround.Around[iCntCur].iX &&
				OldSectorAround.Around[iCntOld].iY == CurSectorAround.Around[iCntCur].iY)
			{
				bFind = true;
				break;
			}
		}
		if (bFind == false)
		{
			pAddSector->Around[pAddSector->iCount] = CurSectorAround.Around[iCntCur];
			pAddSector->iCount++;
		}
	}
}


