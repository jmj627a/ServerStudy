#pragma once
#include <stdio.h>

constexpr int BUFFER_SIZE = 100;

class RingBuf
{
private:
	char arr[BUFFER_SIZE]{ 0, };
	int readPos = 0;	//front
	int writePos = 0;	//rear

private: void Initial(int iBufferSize);

public:
	//생성자
	RingBuf(void);
	RingBuf(int iBufferSize);
	
	//사이즈 재설정, 거의 안 씀
	void Resize(int size);
	
	//버퍼 사이즈 얻기
	int GetBufferSize(void);

	// 현재 사용중인 용량 얻기 / 사용중인 용량 리턴
	int GetUseSize(void);

	// 현재 버퍼에 남은 용량 얻기. / 버퍼에 남은 용량 리턴
	int GetFreeSize(void);



	// WritePos 에 데이타 넣음. / 넣은 크기 리턴 ((char *)데이타 포인터. (int)크기.) //넣은 크기
	int Enqueue(char* chpData, int iSize);

	// ReadPos 에서 데이타 가져옴. ReadPos 이동. ((char *)데이타 포인터. (int)크기.) //가져온 크기
	int Dequeue(char* chpDest, int iSize);

	// ReadPos 에서 데이타 읽어옴. ReadPos 고정. ((char *)데이타 포인터. (int)크기.) //가져온 크기
	int Peek(char* chpDest, int iSize);


	// 원하는 길이만큼 읽기위치 에서 삭제 / 쓰기 위치 이동
	void MoveRear(int iSize);
	int MoveFront(int iSize);

	// 버퍼의 모든 데이타 삭제.
	void ClearBuffer(void);


	// 버퍼의 Front 포인터 얻음. / 버퍼 포인터 리턴 
	char* GetFrontBufferPtr(void);


	// 버퍼의 RearPos 포인터 얻음. / 버퍼 포인터 리턴
	char* GetRearBufferPtr(void);


	/////////////////////////////////////////////////////////////////////////
	// 버퍼 포인터로 외부에서 한방에 읽고, 쓸 수 있는 길이.
	// (끊기지 않은 길이)
	//
	// 원형 큐의 구조상 버퍼의 끝단에 있는 데이터는 끝 -> 처음으로 돌아가서
	// 2번에 데이터를 얻거나 넣을 수 있음. 이 부분에서 끊어지지 않은 길이를 의미
	//
	// Parameters: 없음.
	// Return: (int)사용가능 용량.
	////////////////////////////////////////////////////////////////////////
	int DirectEnqueueSize(void);
	int DirectDequeueSize(void);

};

