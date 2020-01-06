#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string>

//�� ��ŷ ������ �⺻ ���
struct PackHeader
{
	unsigned int code		= 0x0627;		//������ �ڵ�
	unsigned long FileNum = 1;			//������ ��� ����ִ°�?
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
	file[1].offset = 296 + file[0].size;

	fclose(saveFile);


	printf("����° ���� �̸� : ");
	scanf("%s", saveFileName);
	saveFile = fopen(saveFileName, "rb");

	//���� ������ �б�
	fseek(saveFile, 0, SEEK_END);
	size = ftell(saveFile);

	strcpy(file[2].FileName, saveFileName); //�����̸� ����
	file[2].size = size;					//����ũ�� ����
	file[2].offset = 296 + file[0].size + file[1].size;

	fclose(saveFile);


	printf("�׹�° ���� �̸� : ");
	scanf("%s", saveFileName);
	saveFile = fopen(saveFileName, "rb");

	//���� ������ �б�
	fseek(saveFile, 0, SEEK_END);
	size = ftell(saveFile);

	strcpy(file[3].FileName, saveFileName); //�����̸� ����
	file[3].size = size;					//����ũ�� ����
	file[3].offset = 296 + file[0].size + file[1].size + file[2].size;

	fclose(saveFile);



	//���� �����ŭ �Ҵ� 
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




	////���� ������ �� ������ ������
	//rewind(pFile);

	////���� �о pBuffer�� ����
	//int temp = fread(pBuffer, 1, size, pFile);
	//pBuffer[size] = '\0';

	//fwrite(pBuffer, 1, size, pFile);




	//

	//printf("%d %s \n", temp, pBuffer);

	//free(pBuffer);
	fclose(pFile);
}