#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string>

//내 패킹 파일의 기본 헤더
struct PackHeader
{
	unsigned int code		= 0x0627;		//나만의 코드
	unsigned long FileNum = 1;			//파일이 몇개나 들어있는가?
};

struct PackFile
{
	char FileName[64];
	unsigned long size;
	unsigned long offset;
};

void main()
{
	FILE* pFile = fopen("packing.txt", "wb");



	char saveFileName[64]; //파일 이름 입력받을 변수
	PackFile file[4];	   //파일 구조체 배열
	PackHeader header;
	
	printf("첫번째 파일 이름 : ");
	scanf("%s", saveFileName);
	FILE* saveFile = fopen(saveFileName, "rb");

	//파일 사이즈 읽기
	fseek(saveFile, 0, SEEK_END);
	int size = ftell(saveFile);

	strcpy(file[0].FileName, saveFileName); //파일이름 저장
	file[0].size = size;					//파일크기 저장
	file[0].offset = 296;
	
	fclose(saveFile);

	printf("두번째 파일 이름 : ");
	scanf("%s", saveFileName);
	saveFile = fopen(saveFileName, "rb");

	//파일 사이즈 읽기
	fseek(saveFile, 0, SEEK_END);
	size = ftell(saveFile);

	strcpy(file[1].FileName, saveFileName); //파일이름 저장
	file[1].size = size;					//파일크기 저장
	file[1].offset = 296 + file[0].size;

	fclose(saveFile);


	printf("세번째 파일 이름 : ");
	scanf("%s", saveFileName);
	saveFile = fopen(saveFileName, "rb");

	//파일 사이즈 읽기
	fseek(saveFile, 0, SEEK_END);
	size = ftell(saveFile);

	strcpy(file[2].FileName, saveFileName); //파일이름 저장
	file[2].size = size;					//파일크기 저장
	file[2].offset = 296 + file[0].size + file[1].size;

	fclose(saveFile);


	printf("네번째 파일 이름 : ");
	scanf("%s", saveFileName);
	saveFile = fopen(saveFileName, "rb");

	//파일 사이즈 읽기
	fseek(saveFile, 0, SEEK_END);
	size = ftell(saveFile);

	strcpy(file[3].FileName, saveFileName); //파일이름 저장
	file[3].size = size;					//파일크기 저장
	file[3].offset = 296 + file[0].size + file[1].size + file[2].size;

	fclose(saveFile);



	//파일 사이즈만큼 할당 
	char* pBuffer = (char*)malloc(sizeof(PackFile) * 4 + sizeof(PackHeader));
	memset(pBuffer, 0, sizeof(PackFile) * 4 + sizeof(PackHeader));

	fwrite(&header.code, 1, sizeof(unsigned int), pFile);
	fwrite(&header.FileNum, 1, sizeof(unsigned long), pFile);

	for (int i = 0; i < 4; ++i)
	{
		fwrite(&file[i].FileName, 1, sizeof(char) * 64, pFile);
		fwrite(&file[i].size, 1, sizeof(unsigned long), pFile);
		fwrite(&file[i].offset, 1, sizeof(unsigned long), pFile);
	}




	////파일 포인터 맨 앞으로 돌리기
	//rewind(pFile);

	////파일 읽어서 pBuffer에 저장
	//int temp = fread(pBuffer, 1, size, pFile);
	//pBuffer[size] = '\0';

	//fwrite(pBuffer, 1, size, pFile);




	//

	//printf("%d %s \n", temp, pBuffer);

	//free(pBuffer);
	fclose(pFile);
}