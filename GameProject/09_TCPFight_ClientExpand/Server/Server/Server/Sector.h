#pragma once
#include "protocol.h"
#include "global.h"
#include "Content.h"
#include "ServerPrint.h"

struct st_SECTOR_AROUND;
class st_CHARACTER;

#define dfSECTOR_WIDTH 180
#define dfSECTOR_HEIGHT 120

#define dfSECTOR_MAX_X  (dfRANGE_MOVE_RIGHT / dfSECTOR_WIDTH) + 1
#define dfSECTOR_MAX_Y	(dfRANGE_MOVE_BOTTOM / dfSECTOR_HEIGHT) + 1



//ĳ������ ���� ��ǥ x, y�� ������ġ�� ����Ͽ� �ش� ���Ϳ� ����
void Sector_AddCharacter(st_CHARACTER *pCharacter);

//ĳ������ ���� ��ǥ x, y�� ���͸� ����Ͽ� �ش� ���Ϳ��� ����
void Sector_RemoveCharacter(st_CHARACTER *pCharacter);

//���� RemoveCharacter, AddCharacter�� ����Ͽ�
//���� ��ġ�� ���Ϳ��� ���� �� ������ ��ǥ�� ���͸� ���� ����Ͽ� �ش� ���Ϳ� ����
//1.cursector -> oldsector�� ����
//2.cursector ���Ϳ��� ĳ���� ���� remove Character
//3.shX shY�� �ű� ���� ��� -> curSector����
//4.curSector��ġ�� ĳ���� �߰� AddCharacter
bool Sector_UpdateCharacter(st_CHARACTER *pCharacter);

//Ư�� ���� ��ǥ ���� �ֺ� ����� ���� ���
void GetSectorAround(int iSectorX, int iSectorY, st_SECTOR_AROUND *pSectorAround);

//���Ϳ��� ���͸� �̵��Ͽ��� �� ���� ����ǿ��� ���� ����, ���� �߰��� ������ ���� ���ϴ� �Լ�
void GetUpdateSectorAround(st_CHARACTER *pCharacter, st_SECTOR_AROUND *pRemoveSector, st_SECTOR_AROUND *pAddSector);

