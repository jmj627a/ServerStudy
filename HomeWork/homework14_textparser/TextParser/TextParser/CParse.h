#pragma once
class CParse
{
private:
	char* fileBackUpBuffer;			//한번 다 읽으면 처음위치로 포인터 위치 돌리기
	char* fileBuffer;				
	int fileSize; 


public:
	void LoadFile(const char* fileName);
	bool GetValue(const char* szName, int* ipValue); //int부분 템플릿으로? void로?
	
	bool CompareWord(const char* szName, char*& chppBuffer, int* ipLength); //첫번째 두번째 인자를 세번째 인자의 길이만큼 비교
	bool CheckNextWord(char*& chppBuffer, int* ipLength); //의미 있는 단어 나올때까지 돌림
	bool isFindEqual(char*& chppBuffer);  // = 표시 나올때까지 돌림
	bool SkipNoneCommand(char*& chppBuffer); // 탭, 공백, \r\n 다 넘기기
	bool GetStringWord(char** chppBuffer, int* ipLength);
};

