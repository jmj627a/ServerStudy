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
	//������
	RingBuf(void);
	RingBuf(int iBufferSize);
	
	//������ �缳��
	void Resize(int iBufferSize);
	
	//���� ������ ���
	int GetBufferSize(void);
	// ���� ������� �뷮 ��� / ������� �뷮 ����
	int GetUseSize(void);
	// ���� ���ۿ� ���� �뷮 ���. / ���ۿ� ���� �뷮 ����
	int GetFreeSize(void);

	// WritePos �� ������ ����. / ���� ũ�� ���� ((char *)������ ������. (int)ũ��.) //���� ũ��
	int Enqueue(char* chpData, int iSize);
	// ReadPos ���� ������ ������. ReadPos �̵�. ((char *)������ ������. (int)ũ��.) //������ ũ��
	int Dequeue(char* chpDest, int iSize);
	// ReadPos ���� ������ �о��. ReadPos ����. ((char *)������ ������. (int)ũ��.) //������ ũ��
	int Peek(char* chpDest, int iSize);


	// ���ϴ� ���̸�ŭ �б���ġ ���� ���� / ���� ��ġ �̵�
	int MoveRear(int iSize);
	int MoveFront(int iSize);

	// ������ ��� ������ ����.
	void ClearBuffer(void);

	char* GetBufferPtr(void);
	// ������ Front ������ ����. / ���� ������ ���� 
	char* GetFrontBufferPtr(void);
	// ������ RearPos ������ ����. / ���� ������ ����
	char* GetRearBufferPtr(void);

	// ���� �����ͷ� �ܺο��� �ѹ濡 �а�, �� �� �ִ� ����.
	// (������ ���� ����)
	// ���� ť�� ������ ������ ���ܿ� �ִ� �����ʹ� �� -> ó������ ���ư���
	// 2���� �����͸� ��ų� ���� �� ����. �� �κп��� �������� ���� ���̸� �ǹ�
	unsigned int DirectEnqueueSize(void);
	unsigned int DirectDequeueSize(void);


	void Lock();
	void Unlock();

	void PrintBufState(void);
};

