#pragma once
class CParse
{
private:
	char* fileBuffer;
	int fileSize; 

	//int Version;
	//int ServerID;
	//char* ServerBindIP;
	//int ServerBindPort;
	//int WorkerThread;
	//int MaxUser;

public:
	void LoadFile(const char* fileName);
	bool GetValue(const char* szName, int* ipValue); //int부분 템플릿으로? void로?
	
	bool GetNextWord(char** chppBuffer, int* ipLength);
	bool SkipNoneCommand(void);
	bool GetStringWord(char** chppBuffer, int* ipLength);
};

