#include "UI.h"
#include "global.h"
#include "CPlayer.h"


void UI::printUI()
{
	//â �׵θ�
	for (int i = 0; i < dfSCREEN_HEIGHT; ++i)
	{
		gotoxy(dfSCREEN_WIDTH , i);
		printf("��");
	}

	gotoxy(0, dfSCREEN_HEIGHT);
	printf("������������������������������������������������������������������������������������������������������������������������������������������������������������������");

	//���
	gotoxy(0, dfSCREEN_HEIGHT + 1);
	printf(" life : ������");

	gotoxy(0, dfSCREEN_HEIGHT + 1);
	printf(" life : ");

	for (int i = 0; i < player->getLife(); ++i)
		printf("��");
}

void UI::deleteUI()
{
	for (int i = 0; i < dfSCREEN_HEIGHT; ++i)
	{
		gotoxy(dfSCREEN_WIDTH, i);
		printf("");
	}

	gotoxy(0, dfSCREEN_HEIGHT);
	printf("                                                                                  ");

	//���
	gotoxy(0, dfSCREEN_HEIGHT + 1);
	printf("                                                                                  ");

}
