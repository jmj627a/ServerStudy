#include "Enemy.h"
#include "Player.h"
#include "Console.h"
#include "Scene.h"
#include "global.h"
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

void enemyInit()
{
	free(g_Enemy);
	g_Enemy = (Enemy*)malloc(sizeof(Enemy) * ENEMY_NUM);

	FILE* pStageFile = fopen("Enemy.txt", "rb");

	fseek(pStageFile, 0, SEEK_END);
	int size = ftell(pStageFile);

	char* pStageBuffer = (char*)malloc(size / g_TotalStage);
	rewind(pStageFile);

	//fseek(pStageFile, 1889 * g_GameStage, SEEK_SET);
	//int temp = fread(pStageBuffer, 1, 1, pStageFile);
	//
	//char temp22 = *pStageBuffer;
	//printf("%c\n",temp22);

	//1886 = 80*23 + 2*23			82 * 23
	fseek(pStageFile, 3 * (g_GameStage + 1) + g_GameStage * 1886, SEEK_SET);

	int temp = fread(pStageBuffer, 1, 1886, pStageFile);
	pStageBuffer[1887] = '\0';

	//printf("%s", pStageBuffer);


	int count = 0;
	for (int i = 0; i < 1886; ++i)
	{
		if (pStageBuffer[i] == '7')
		{
			g_Enemy[count].flag = true;
			g_Enemy[count].x = i % 82;
			g_Enemy[count].y = i / 82;

			g_Enemy[count].startX = g_Enemy[count].x;
			g_Enemy[count].startY = g_Enemy[count].y;
			g_Enemy[count].dir = 0;

			count++;
		}
	}

	//system("pause");
}