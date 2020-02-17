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

	fileBackUpBuffer = fileBuffer;
	printf("%s \n\n", fileBuffer);


	fclose(fp);
}

//szName : 찾고싶은 문자열, ipValue : 문자열에 해당하는 값 받아올 곳
bool CParse::GetValue(const char* szName, int* ipValue)
{
	char* chpBuff = fileBuffer; //어디까지 읽었나 저장하는 포인터
	char chWord[256]; //문자 복사해올 배열
	int	iLength; //문자의 길이

	//의미있는 단어(공백제외문자)가 나올때까지 while로 검사
	while (CheckNextWord(chpBuff, &iLength))
	{
		//한 단어가 나왔다면, 찾고싶은 문자열과 비교
		if (CompareWord(szName, chpBuff, &iLength))
		{
			// 찾고싶은 문자열이 맞다면 바로 뒤에 = 을 찾음. 
			// 등호가 나올때까지 반복
			if (isFindEqual(chpBuff))
			{
				// = 다음의 데이터 부분 얻기.
				if (CheckNextWord(chpBuff, &iLength))
				{
					//얻었으면 복사
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
	char chWord[256]; //문자 복사해올 배열

	// Word 버퍼에 찾은 단어를 저장한다.
	memset(chWord, 0, 256);
	memcpy(chWord, chppBuffer, *ipLength);

	// 인자로 입력 받은 단어와 같은지 검사한다.
	if (0 == strcmp(szName, chWord))
		return true;

	chppBuffer = chppBuffer + *ipLength; //이 단어는 같은 단어가 아니니 위치 건너뛰기
	return false;
}

bool CParse::CheckNextWord(char*& chppBuffer, int* ipLength)
{
	int length = 0;

	//글자 나올때까지 넘기기
	SkipNoneCommand(chppBuffer);

	//주석 끝날때까지 넘기기 //만나면 0a0d 나올때까지 /*만나면 */나올때까지 
	//SkipComment();


	while (1)
	{
		//공백이나 tab, 줄바꿈 이라면 한 단어가 끝
		if (*fileBuffer == ' ' || *fileBuffer == '\t' || *fileBuffer == '\r' || *fileBuffer == '\n')
		{
			fileBuffer = fileBuffer - length;		//지나온 단어 다시 제자리로
			chppBuffer = fileBuffer;				//단어 시작위치
			*ipLength = length;						//단어 길이

			return true;
		}
		else if (*fileBuffer == '"') //여기서 "시작하면 문자열 부르는 함수로 "끝날때까지
		{
		}
		fileBuffer++;
		length++;
	}

	return false;
}

bool CParse::isFindEqual(char*& chppBuffer)
{
	fileBuffer = chppBuffer; //지금 위치 넣기

	while (1)
	{
		//공백이나 tab 이라면 한 단어가 끝
		if (*fileBuffer == '=')
		{
			chppBuffer = ++fileBuffer;				//= 다음 단어 시작위치
			SkipNoneCommand(chppBuffer);			//공백있다면 건너뛰고 포인터 주기
			return true;
		}
		fileBuffer++;
	}

	return false;
}

bool CParse::SkipNoneCommand(char*& chppBuffer)
{
	fileBuffer = chppBuffer; //지금 위치 넣기

	while (1)
	{
		//공백이라면 계속 전진, 아니라면 그 위치 넣어주기
		if (*fileBuffer == ' ' || *fileBuffer == '\t' || *fileBuffer == '\r' || *fileBuffer == '\n')
		{
			fileBuffer++;
		}
		else if (*fileBuffer == '/' && *(fileBuffer + 1) == '/') //주석//표시 한줄 무시
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
		else if (*fileBuffer == '/' && *(fileBuffer + 1) == '*') //주석/* */ 표시 무시
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
