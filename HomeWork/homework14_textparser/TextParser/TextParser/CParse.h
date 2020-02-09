#pragma once
class CParse
{
private:
	char* fileBackUpBuffer;			//�ѹ� �� ������ ó����ġ�� ������ ��ġ ������
	char* fileBuffer;				
	int fileSize; 


public:
	void LoadFile(const char* fileName);
	bool GetValue(const char* szName, int* ipValue); //int�κ� ���ø�����? void��?
	
	bool CompareWord(const char* szName, char*& chppBuffer, int* ipLength); //ù��° �ι�° ���ڸ� ����° ������ ���̸�ŭ ��
	bool CheckNextWord(char*& chppBuffer, int* ipLength); //�ǹ� �ִ� �ܾ� ���ö����� ����
	bool isFindEqual(char*& chppBuffer);  // = ǥ�� ���ö����� ����
	bool SkipNoneCommand(char*& chppBuffer); // ��, ����, \r\n �� �ѱ��
	bool GetStringWord(char** chppBuffer, int* ipLength);
};

