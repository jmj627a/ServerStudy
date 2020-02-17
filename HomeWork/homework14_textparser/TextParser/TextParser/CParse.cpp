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

	fileBackUpBuffer = fileBuffer;
	printf("%s \n\n", fileBuffer);


	fclose(fp);
}

//szName : ã����� ���ڿ�, ipValue : ���ڿ��� �ش��ϴ� �� �޾ƿ� ��
bool CParse::GetValue(const char* szName, int* ipValue)
{
	char* chpBuff = fileBuffer; //������ �о��� �����ϴ� ������
	char chWord[256]; //���� �����ؿ� �迭
	int	iLength; //������ ����

	//�ǹ��ִ� �ܾ�(�������ܹ���)�� ���ö����� while�� �˻�
	while (CheckNextWord(chpBuff, &iLength))
	{
		//�� �ܾ ���Դٸ�, ã����� ���ڿ��� ��
		if (CompareWord(szName, chpBuff, &iLength))
		{
			// ã����� ���ڿ��� �´ٸ� �ٷ� �ڿ� = �� ã��. 
			// ��ȣ�� ���ö����� �ݺ�
			if (isFindEqual(chpBuff))
			{
				// = ������ ������ �κ� ���.
				if (CheckNextWord(chpBuff, &iLength))
				{
					//������� ����
					memset(chWord, 0, 256);
					memcpy(chWord, chpBuff, iLength);
					*ipValue = atoi(chWord);
					fileBuffer = fileBackUpBuffer;
					return true;
				}
				return false;
			}
			return false;
		}
		continue;
	}
	return false;

}

bool CParse::CompareWord(const char* szName, char*& chppBuffer, int* ipLength)
{
	char chWord[256]; //���� �����ؿ� �迭

	// Word ���ۿ� ã�� �ܾ �����Ѵ�.
	memset(chWord, 0, 256);
	memcpy(chWord, chppBuffer, *ipLength);

	// ���ڷ� �Է� ���� �ܾ�� ������ �˻��Ѵ�.
	if (0 == strcmp(szName, chWord))
		return true;

	chppBuffer = chppBuffer + *ipLength; //�� �ܾ�� ���� �ܾ �ƴϴ� ��ġ �ǳʶٱ�
	return false;
}

bool CParse::CheckNextWord(char*& chppBuffer, int* ipLength)
{
	int length = 0;

	//���� ���ö����� �ѱ��
	SkipNoneCommand(chppBuffer);

	//�ּ� ���������� �ѱ�� //������ 0a0d ���ö����� /*������ */���ö����� 
	//SkipComment();


	while (1)
	{
		//�����̳� tab, �ٹٲ� �̶�� �� �ܾ ��
		if (*fileBuffer == ' ' || *fileBuffer == '\t' || *fileBuffer == '\r' || *fileBuffer == '\n')
		{
			fileBuffer = fileBuffer - length;		//������ �ܾ� �ٽ� ���ڸ���
			chppBuffer = fileBuffer;				//�ܾ� ������ġ
			*ipLength = length;						//�ܾ� ����

			return true;
		}
		else if (*fileBuffer == '"') //���⼭ "�����ϸ� ���ڿ� �θ��� �Լ��� "����������
		{
		}
		fileBuffer++;
		length++;
	}

	return false;
}

bool CParse::isFindEqual(char*& chppBuffer)
{
	fileBuffer = chppBuffer; //���� ��ġ �ֱ�

	while (1)
	{
		//�����̳� tab �̶�� �� �ܾ ��
		if (*fileBuffer == '=')
		{
			chppBuffer = ++fileBuffer;				//= ���� �ܾ� ������ġ
			SkipNoneCommand(chppBuffer);			//�����ִٸ� �ǳʶٰ� ������ �ֱ�
			return true;
		}
		fileBuffer++;
	}

	return false;
}

bool CParse::SkipNoneCommand(char*& chppBuffer)
{
	fileBuffer = chppBuffer; //���� ��ġ �ֱ�

	while (1)
	{
		//�����̶�� ��� ����, �ƴ϶�� �� ��ġ �־��ֱ�
		if (*fileBuffer == ' ' || *fileBuffer == '\t' || *fileBuffer == '\r' || *fileBuffer == '\n')
		{
			fileBuffer++;
		}
		else if (*fileBuffer == '/' && *(fileBuffer + 1) == '/') //�ּ�//ǥ�� ���� ����
		{
			while (true)
			{
				fileBuffer++;

				if (*fileBuffer == '\r' && *(fileBuffer + 1) == '\n')
				{
					fileBuffer += 2;
					break;
				}

			}
		}
		else if (*fileBuffer == '/' && *(fileBuffer + 1) == '*') //�ּ�/* */ ǥ�� ����
		{
			while (true)
			{
				fileBuffer++;

				if (*fileBuffer == '*' && *(fileBuffer + 1) == '/')
				{
					fileBuffer += 2;
					break;
				}

			}
		}
		else
		{
			chppBuffer = fileBuffer;				
			return true;
		}

	}

	return false;
}
