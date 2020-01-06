struct PackHeader
{
	//내 패킹파일의 기본헤더
	unsigned int code; //나만의 코드
	unsigned long FileNum; // 파일이 몇개나 있을지. 결국 헤더의 size가 나옴

};

//이건 위의 FileNum만큼 반복된다.
struct PackFile
{
	char FileName[64];
	unsigned long size;
	unsigned long offset;
};