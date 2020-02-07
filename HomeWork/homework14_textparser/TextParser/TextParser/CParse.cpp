#include "CParse.h"
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

void CParse::LoadFile(const char* fileName)
{
	errno_t err;
	FILE* fp;
	
	err = fopen_s(&fp, fileName, "rb");
	if (err == 0)	printf("파일오픈 성공!\n");
	else {		printf("파일 오픈 실패\n"); return; }
	

	fseek(fp, 0, SEEK_END);		// 파일 포인터를 파일의 끝으로 이동시킴
	fileSize = ftell(fp);       // 파일 포인터의 현재 위치를 얻음
	fseek(fp, 0, SEEK_SET);		// 파일 포인터 위치 맨 앞으로

	fileBuffer = (char*)malloc(fileSize); //버퍼 할당받고 파일 읽어오기
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
	// 찾고자 하는 단어가 나올때까지 계속 찾을 것이므로 while 문으로 검사.
	while (GetNextWord(&chpBuff, &iLength))
	{
		// Word 버퍼에 찾은 단어를 저장한다.
		memset(chWord, 0, 256);
		memcpy(chWord, chpBuff, iLength);

		//printf(">>>>>>%s<<<<<< \n", chWord);

		// 인자로 입력 받은 단어와 같은지 검사한다.
		if (0 == strcmp(szName, chpBuff))
		{
			// 맞다면 바로 뒤에 = 을 찾자.
			if (GetNextWord(&chpBuff, &iLength))
			{
				memset(chWord, 0, 256);
				memcpy(chWord, chpBuff, iLength);
				if (0 == strcmp(chpBuff, "="))
				{
					// = 다음의 데이터 부분을 얻자.
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
		//공백이나 tab 이라면 한 단어가 끝
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
