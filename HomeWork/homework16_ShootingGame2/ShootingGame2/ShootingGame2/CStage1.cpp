#include "CStage1.h"
#include "global.h"

void CStage1::EnemyInit()
{
	cs_ClearScreen();

	FILE* pStageFile = fopen("Enemy.txt", "rb");

	fseek(pStageFile, 0, SEEK_END);
	int size = ftell(pStageFile);

	char* pStageBuffer = (char*)malloc(size / STAGE_NUM);
	rewind(pStageFile);


	//1886 = 80*23 + 2*23			82 * 23
	//3 * STAGE + (STAGE-1) * 1886
	fseek(pStageFile, 3 * 1 + 0 * 1886, SEEK_SET);

	int temp = fread(pStageBuffer, 1, 1886, pStageFile);
	pStageBuffer[1887] = '\0';

	//printf("%s", pStageBuffer);

	for (int i = 0; i < 1886; ++i)
	{
		if (pStageBuffer[i] == '7')
		{
			objectManager->CreateObject(ENEMY, i % 82, i / 82, i % 82, i / 82, 0);
		}
	}

	free(pStageBuffer);
}


