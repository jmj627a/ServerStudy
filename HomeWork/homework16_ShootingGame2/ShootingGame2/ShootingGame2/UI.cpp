#include "UI.h"
#include "global.h"
#include "CPlayer.h"


void UI::printUI()
{
	//芒 抛滴府
	for (int i = 0; i < dfSCREEN_HEIGHT; ++i)
	{
		gotoxy(dfSCREEN_WIDTH , i);
		printf("Β");
	}

	gotoxy(0, dfSCREEN_HEIGHT);
	printf("ΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΑΕ");

	//格见
	gotoxy(0, dfSCREEN_HEIGHT + 1);
	printf(" life : ⒔⒔⒔");

	gotoxy(0, dfSCREEN_HEIGHT + 1);
	printf(" life : ");

	for (int i = 0; i < player->getLife(); ++i)
		printf("⒕");
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

	//格见
	gotoxy(0, dfSCREEN_HEIGHT + 1);
	printf("                                                                                  ");

}
