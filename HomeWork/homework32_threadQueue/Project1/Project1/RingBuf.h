#pragma once
#include <stdio.h>
#include <string>
#include <Windows.h>

constexpr int BUFFER_SIZE = 40000;
constexpr int DATA_SIZE = 121;

#define READ		0
#define WRITE		1

class RingBuf
{
public:
	char arr[BUFFER_SIZE]{ 0, };
	int readPos = 0;	//front
	int writePos = 0;	//rear

	int count = 0;
private: void Initial(int iBufferSize);

private: 
	SRWLOCK srw;

public:
	//������
	RingBuf(void);
	RingBuf(int iBufferSize);

	~RingBuf();

	//������ �缳��, ���� �� ��
	void Resize(int size);

	//���� ������ ���
	int GetBufferSize(void);

	// ���� ������� �뷮 ��� / ������� �뷮 ����
	int GetUseSize(void);

	// ���� ���ۿ� ���� �뷮 ���. / ���ۿ� ���� �뷮 ����
	int GetFreeSize(void);

	void RemoveData(int iSize);

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


	// ������ Front ������ ����. / ���� ������ ���� 
	char* GetFrontBufferPtr(void);


	// ������ RearPos ������ ����. / ���� ������ ����
	char* GetRearBufferPtr(void);


	char* GetBufferPtr(void);

	/////////////////////////////////////////////////////////////////////////
	// ���� �����ͷ� �ܺο��� �ѹ濡 �а�, �� �� �ִ� ����.
	// (������ ���� ����)
	//
	// ���� ť�� ������ ������ ���ܿ� �ִ� �����ʹ� �� -> ó������ ���ư���
	// 2���� �����͸� ��ų� ���� �� ����. �� �κп��� �������� ���� ���̸� �ǹ�
	//
	// Parameters: ����.
	// Return: (int)��밡�� �뷮.
	////////////////////////////////////////////////////////////////////////
	int DirectEnqueueSize(void);
	int DirectDequeueSize(void);


	//0 - read / 1 - write
	void lock(bool type);
	void unlock(bool type);

};


