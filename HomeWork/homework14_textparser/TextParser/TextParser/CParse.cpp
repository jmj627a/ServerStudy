#include "CParse.h"
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

void CParse::LoadFile(const char* fileName)
{
	errno_t err;
	FILE* fp;
	
	err = fopen_s(&fp, fileName, "rb");
	if (err == 0)	printf("���Ͽ��� ����!\n");
	else {		printf("���� ���� ����\n"); return; }
	

	fseek(fp, 0, SEEK_END);		// ���� �����͸� ������ ������ �̵���Ŵ
	fileSize = ftell(fp);       // ���� �������� ���� ��ġ�� ����
	fseek(fp, 0, SEEK_SET);		// ���� ������ ��ġ �� ������

	fileBuffer = (char*)malloc(fileSize); //���� �Ҵ�ް� ���� �о����
	int ret = fread(fileBuffer, 1, fileSize, fp);
	if (fileSize != ret)
		return;

	printf("%s \n\n", fileBuffer);


	fclose(fp);
}

bool CParse::GetValue(const char* szName, int* ipValue)
{
	char* chpBuff = (char*)malloc(fileSize);
	memcpy(chpBuff, fileBuffer, fileSize);
	char chWord[256];
	int	iLength;
	// ã���� �ϴ� �ܾ ���ö����� ��� ã�� ���̹Ƿ� while ������ �˻�.
	while (GetNextWord(&chpBuff, &iLength))
	{
		// Word ���ۿ� ã�� �ܾ �����Ѵ�.
		memset(chWord, 0, 256);
		memcpy(chWord, chpBuff, iLength);

		//printf(">>>>>>%s<<<<<< \n", chWord);

		// ���ڷ� �Է� ���� �ܾ�� ������ �˻��Ѵ�.
		if (0 == strcmp(szName, chpBuff))
		{
			// �´ٸ� �ٷ� �ڿ� = �� ã��.
			if (GetNextWord(&chpBuff, &iLength))
			{
				memset(chWord, 0, 256);
				memcpy(chWord, chpBuff, iLength);
				if (0 == strcmp(chpBuff, "="))
				{
					// = ������ ������ �κ��� ����.
					if (GetNextWord(&chpBuff, &iLength))
					{
						memset(chWord, 0, 256);
						memcpy(chWord, chpBuff, iLength);
						*ipValue = atoi(chWord);
						return true;
					}
					return false;
				}
			}
			return false;
		}
	}
	return false;

}

bool CParse::GetNextWord(char** chppBuffer, int* ipLength)
{
	//printf("%s \n", *chppBuffer);
	int length = 0;

	//chppBuffer = &fileBuffer;
	//*ipLength = 7;
	////printf(" >>%c, %d<<\n", *chppBuffer, *ipLength);
	////break;
	//return true;

	while (1)
	{
		//�����̳� tab �̶�� �� �ܾ ��
		if (*fileBuffer == 0x09 || *fileBuffer == 0x20)
		{
			chppBuffer = &fileBuffer;
			*ipLength = length;
			//printf(" >>%c, %d<<\n", *chppBuffer, *ipLength);
			break;
		}
		//printf("%c\n", *fileBuffer);
		fileBuffer++;
		length++;
	}


	return false;
}
