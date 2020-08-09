#pragma once
#include <stdio.h>
#include <Windows.h>

class RingBuf
{
private:
	char* dataBuf;
	int readPos;		//front
	int writePos;		//rear
	int totalSize;
	int usedSize;

	CRITICAL_SECTION cs;

private:
	void Initialize(int iBufferSize);
	void Finalize(void);

public:
	//생성자
	RingBuf(void);
	RingBuf(int iBufferSize);
	
	//사이즈 재설정
	void Resize(int iBufferSize);
	
	//버퍼 사이즈 얻기
	int GetBufferSize(void);
	// 현재 사용중인 용량 얻기 / 사용중인 용량 리턴
	int GetUseSize(void);
	// 현재 버퍼에 남은 용량 얻기. / 버퍼에 남은 용량 리턴
	int GetFreeSize(void);

	// WritePos 에 데이터 넣음. / 넣은 크기 리턴 ((char *)데이터 포인터. (int)크기.) //넣은 크기
	int Enqueue(char* chpData, int iSize);
	// ReadPos 에서 데이터 가져옴. ReadPos 이동. ((char *)데이터 포인터. (int)크기.) //가져온 크기
	int Dequeue(char* chpDest, int iSize);
	// ReadPos 에서 데이터 읽어옴. ReadPos 고정. ((char *)데이터 포인터. (int)크기.) //가져온 크기
	int Peek(char* chpDest, int iSize);


	// 원하는 길이만큼 읽기위치 에서 삭제 / 쓰기 위치 이동
	int MoveRear(int iSize);
	int MoveFront(int iSize);

	// 버퍼의 모든 데이터 삭제.
	void ClearBuffer(void);

	char* GetBufferPtr(void);
	// 버퍼의 Front 포인터 얻음. / 버퍼 포인터 리턴 
	char* GetFrontBufferPtr(void);
	// 버퍼의 RearPos 포인터 얻음. / 버퍼 포인터 리턴
	char* GetRearBufferPtr(void);

	// 버퍼 포인터로 외부에서 한방에 읽고, 쓸 수 있는 길이.
	// (끊기지 않은 길이)
	// 원형 큐의 구조상 버퍼의 끝단에 있는 데이터는 끝 -> 처음으로 돌아가서
	// 2번에 데이터를 얻거나 넣을 수 있음. 이 부분에서 끊어지지 않은 길이를 의미
	unsigned int DirectEnqueueSize(void);
	unsigned int DirectDequeueSize(void);


	void Lock();
	void Unlock();

	void PrintBufState(void);
};

