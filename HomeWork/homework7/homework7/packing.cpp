#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <conio.h>

//�� ��ŷ ������ �⺻ ���
struct PackHeader
{
	unsigned int code		= 0x06270627;		//������ �ڵ�
	unsigned long FileNum = 4;			//������ ��� ����ִ°�?
};

struct PackFile
{
	char FileName[64];
	unsigned long size;
	unsigned long offset;
};

void main()
{
	printf("1. ��ŷ�ϱ� \t\t 2. ��ŷǮ��  \n");
	switch (_getch())
	{

	case '1':
	{
		//==========================
		//		  packing
		//==========================


		FILE* pFile = fopen("packing.txt", "wb");

		char saveFileName[64]; //���� �̸� �Է¹��� ����
		PackFile file[4];	   //���� ����ü �迭
		PackHeader header;

		printf("ù��° ���� �̸� : ");
		scanf("%s", saveFileName);
		FILE* saveFile = fopen(saveFileName, "rb");

		//���� ������ �б�
		fseek(saveFile, 0, SEEK_END);
		int size = ftell(saveFile);

		strcpy(file[0].FileName, saveFileName); //�����̸� ����
		file[0].size = size;					//����ũ�� ����
		file[0].offset = 296;

		fclose(saveFile);

		printf("�ι�° ���� �̸� : ");
		scanf("%s", saveFileName);
		saveFile = fopen(saveFileName, "rb");

		//���� ������ �б�
		fseek(saveFile, 0, SEEK_END);
		size = ftell(saveFile);

		strcpy(file[1].FileName, saveFileName); //�����̸� ����
		file[1].size = size;					//����ũ�� ����
		file[1].offset = file[0].offset + file[0].size;

		fclose(saveFile);


		printf("����° ���� �̸� : ");
		scanf("%s", saveFileName);
		saveFile = fopen(saveFileName, "rb");

		//���� ������ �б�
		fseek(saveFile, 0, SEEK_END);
		size = ftell(saveFile);

		strcpy(file[2].FileName, saveFileName); //�����̸� ����
		file[2].size = size;					//����ũ�� ����
		file[2].offset = file[1].offset + file[1].size;

		fclose(saveFile);


		printf("�׹�° ���� �̸� : ");
		scanf("%s", saveFileName);
		saveFile = fopen(saveFileName, "rb");

		//���� ������ �б�
		fseek(saveFile, 0, SEEK_END);
		size = ftell(saveFile);

		strcpy(file[3].FileName, saveFileName); //�����̸� ����
		file[3].size = size;					//����ũ�� ����
		file[3].offset = file[2].offset + file[2].size;

		fclose(saveFile);



		//���� �����ŭ �Ҵ� 
		char* pBuffer = (char*)malloc(sizeof(PackFile) * 4 + sizeof(PackHeader));
		memset(pBuffer, 0, sizeof(PackFile) * 4 + sizeof(PackHeader));

		//��� ����
		fwrite(&header.code, 1, sizeof(unsigned int), pFile);
		fwrite(&header.FileNum, 1, sizeof(unsigned long), pFile);

		//���� �̸�, ������, offset ����
		for (int i = 0; i < 4; ++i)
		{
			fwrite(&file[i].FileName, 1, sizeof(char) * 64, pFile);
			fwrite(&file[i].size, 1, sizeof(unsigned long), pFile);
			fwrite(&file[i].offset, 1, sizeof(unsigned long), pFile);
		}
		free(pBuffer);


		//���� ������ ����
		for (int i = 0; i < 4; ++i)
		{
			saveFile = fopen(file[i].FileName, "rb");

			//���� ������ �б�
			fseek(saveFile, 0, SEEK_END);
			int size = ftell(saveFile);
			//printf("%d \n", size);

			//���� �����ŭ �Ҵ�
			char* pBuffer = (char*)malloc(size);
			memset(pBuffer, 0, size);

			//���� ������ �� ������ ������
			rewind(saveFile);

			//���� �о pBuffer�� ����
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

		//���� ������ �б�
		fseek(pFile, 0, SEEK_END);
		int size = ftell(pFile);
		//printf("%d \n", size);

		//���� �����ŭ �Ҵ�
		char* pBuffer = (char*)malloc(size);
		memset(pBuffer, 0, size);

		//���� ������ �� ������ ������
		rewind(pFile);

		//���� �о pBuffer�� ����
		int temp = fread(pBuffer, 1, size, pFile);
		//pBuffer[size] = '\0';

		//printf("%d %s \n", temp, pBuffer);

		fread(pBuffer, 1, size, pFile);


		//pBuffer�� ������ �� �Űܿ���

		char saveFileName[64]; //���� �̸� �Է¹��� ����
		PackFile file;	   //���� ����ü
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

			//file �ϳ��� ����ü�� ���� �̸�, ������, offset ����
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