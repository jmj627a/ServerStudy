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



//캐릭터의 현재 좌표 x, y로 섹터위치를 계산하여 해당 섹터에 넣음
void Sector_AddCharacter(st_CHARACTER *pCharacter);

//캐릭터의 현재 좌표 x, y로 섹터를 계산하여 해당 섹터에서 삭제
void Sector_RemoveCharacter(st_CHARACTER *pCharacter);

//위의 RemoveCharacter, AddCharacter를 사용하여
//현재 위치한 섹터에서 삭제 후 현재의 좌표로 섹터를 새로 계산하여 해당 섹터에 넣음
//1.cursector -> oldsector에 저장
//2.cursector 섹터에서 캐릭터 삭제 remove Character
//3.shX shY로 신규 섹터 계산 -> curSector저장
//4.curSector위치에 캐릭터 추가 AddCharacter
bool Sector_UpdateCharacter(st_CHARACTER *pCharacter);

//특정 섹터 좌표 기준 주변 영향권 섹터 얻기
void GetSectorAround(int iSectorX, int iSectorY, st_SECTOR_AROUND *pSectorAround);

//섹터에서 섹터를 이동하였을 때 섹터 영향권에서 빠진 섹터, 새로 추가된 섹터의 정보 구하는 함수
void GetUpdateSectorAround(st_CHARACTER *pCharacter, st_SECTOR_AROUND *pRemoveSector, st_SECTOR_AROUND *pAddSector);

