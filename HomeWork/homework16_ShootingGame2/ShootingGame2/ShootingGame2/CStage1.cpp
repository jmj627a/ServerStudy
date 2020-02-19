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
}


//이거 PLAYER 안으로 들어가야할거같은데..?????
bool CStage1::checkMessage()
{
	int message;
	int flag = messageQueue->Deq(&message);

	if (flag == false)
		return false;

	//방향키 이동과, 총알 발사에 대한 메세지 분류
	switch (message)
	{
	case KEY_UP:
		break;

	case KEY_DOWN:
		break;

	case KEY_RIGHT:
		break;

	case KEY_LEFT:
		break;

	case KEY_SPACE:
		break;
	}

	return true;
}