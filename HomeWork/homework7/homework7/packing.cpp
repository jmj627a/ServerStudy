#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <conio.h>

//내 패킹 파일의 기본 헤더
struct PackHeader
{
	unsigned int code		= 0x06270627;		//나만의 코드
	unsigned long FileNum = 4;			//파일이 몇개나 들어있는가?
};

struct PackFile
{
	char FileName[64];
	unsigned long size;
	unsigned long offset;
};

void main()
{
	printf("1. 패킹하기 \t\t 2. 패킹풀기  \n");
	switch (_getch())
	{

	case '1':
	{
		//==========================
		//		  packing
		//==========================


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
		file[1].offset = file[0].offset + file[0].size;

		fclose(saveFile);


		printf("세번째 파일 이름 : ");
		scanf("%s", saveFileName);
		saveFile = fopen(saveFileName, "rb");

		//파일 사이즈 읽기
		fseek(saveFile, 0, SEEK_END);
		size = ftell(saveFile);

		strcpy(file[2].FileName, saveFileName); //파일이름 저장
		file[2].size = size;					//파일크기 저장
		file[2].offset = file[1].offset + file[1].size;

		fclose(saveFile);


		printf("네번째 파일 이름 : ");
		scanf("%s", saveFileName);
		saveFile = fopen(saveFileName, "rb");

		//파일 사이즈 읽기
		fseek(saveFile, 0, SEEK_END);
		size = ftell(saveFile);

		strcpy(file[3].FileName, saveFileName); //파일이름 저장
		file[3].size = size;					//파일크기 저장
		file[3].offset = file[2].offset + file[2].size;

		fclose(saveFile);



		//파일 사이즈만큼 할당 
		char* pBuffer = (char*)malloc(sizeof(PackFile) * 4 + sizeof(PackHeader));
		memset(pBuffer, 0, sizeof(PackFile) * 4 + sizeof(PackHeader));

		//헤더 저장
		fwrite(&header.code, 1, sizeof(unsigned int), pFile);
		fwrite(&header.FileNum, 1, sizeof(unsigned long), pFile);

		//파일 이름, 사이즈, offset 저장
		for (int i = 0; i < 4; ++i)
		{
			fwrite(&file[i].FileName, 1, sizeof(char) * 64, pFile);
			fwrite(&file[i].size, 1, sizeof(unsigned long), pFile);
			fwrite(&file[i].offset, 1, sizeof(unsigned long), pFile);
		}
		free(pBuffer);


		//파일 데이터 저장
		for (int i = 0; i < 4; ++i)
		{
			saveFile = fopen(file[i].FileName, "rb");

			//파일 사이즈 읽기
			fseek(saveFile, 0, SEEK_END);
			int size = ftell(saveFile);
			//printf("%d \n", size);

			//파일 사이즈만큼 할당
			char* pBuffer = (char*)malloc(size);
			memset(pBuffer, 0, size);

			//파일 포인터 맨 앞으로 돌리기
			rewind(saveFile);

			//파일 읽어서 pBuffer에 저장
			int temp = fread(pBuffer, 1, size, saveFile);
			//pBuffer[size] = '\0';

			//printf("%d %s \n", temp, pBuffer);

			fclose(saveFile);

			fwrite(pBuffer, 1, size, pFile);

			free(pBuffer);
		}

		fclose(pFile);
		break;
	}



	case '2':
	{
		//==========================
		//		unpacking
		//==========================

		FILE* pFile = fopen("packing.txt", "rb");

		//파일 사이즈 읽기
		fseek(pFile, 0, SEEK_END);
		int size = ftell(pFile);
		//printf("%d \n", size);

		//파일 사이즈만큼 할당
		char* pBuffer = (char*)malloc(size);
		memset(pBuffer, 0, size);

		//파일 포인터 맨 앞으로 돌리기
		rewind(pFile);

		//파일 읽어서 pBuffer에 저장
		int temp = fread(pBuffer, 1, size, pFile);
		//pBuffer[size] = '\0';

		//printf("%d %s \n", temp, pBuffer);

		fread(pBuffer, 1, size, pFile);


		//pBuffer에 내용을 다 옮겨왔음

		char saveFileName[64]; //파일 이름 입력받을 변수
		PackFile file;	   //파일 구조체
		PackHeader header;

		char* pSaveBuffer = (char*)malloc(sizeof(PackHeader));

		fseek(pFile, 0, SEEK_SET);
		fread(pSaveBuffer, 1, sizeof(PackHeader), pFile);

		memcpy(&header, pSaveBuffer, sizeof(PackHeader));
		free(pSaveBuffer);

		for (int i = 0; i < header.FileNum; ++i)
		{
			pSaveBuffer = (char*)malloc(sizeof(PackFile));
			fseek(pFile, sizeof(PackHeader) + sizeof(PackFile) * i, SEEK_SET);
			fread(pSaveBuffer, 1, sizeof(PackFile), pFile);

			//file 하나씩 구조체에 파일 이름, 사이즈, offset 저장
			memcpy(&file, pSaveBuffer, sizeof(PackFile));
			free(pSaveBuffer);

			FILE* pSaveFile = fopen(file.FileName, "wb");
			pSaveBuffer = (char*)malloc(file.size);

			fseek(pFile, file.offset, SEEK_SET);
			//memcpy(pSaveBuffer, pBuffer, file.size);
			fread(pSaveBuffer, 1, file.size, pFile);
			fwrite(pSaveBuffer, 1, file.size, pSaveFile);

			free(pSaveBuffer);
			fclose(pSaveFile);
		}


		free(pBuffer);
		fclose(pFile);
		break;
	}
	}
}