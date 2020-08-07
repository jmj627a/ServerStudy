#include "RingBuf.h"
#include <string>
#include <Windows.h>

void RingBuf::Initialize(int iBufferSize)
{
	Finalize();

	if (iBufferSize > 0)
	{
		dataBuf = new char[iBufferSize];
		memset(dataBuf, 0, iBufferSize);
		readPos = 0;
		writePos = 0;
		totalSize = iBufferSize;
		usedSize = 0;

		InitializeCriticalSection(&cs);
	}
}

void RingBuf::Finalize(void)
{
	if (dataBuf != NULL)
	{
		delete[] dataBuf;
		DeleteCriticalSection(&cs);
	}

	dataBuf = NULL;
	readPos = 0;
	writePos = 0;
	totalSize = 0;
	usedSize = 0;

}

RingBuf::RingBuf(void)
{
	dataBuf = NULL;
	readPos = 0;
	writePos = 0;
	totalSize = 0;
	usedSize = 0;

	Initialize(10000);
}

RingBuf::RingBuf(int iBufferSize)
{
	dataBuf = NULL;
	readPos = 0;
	writePos = 0;
	totalSize = 0;
	usedSize = 0;

	Initialize(iBufferSize);
}

void RingBuf::Resize(int iBufferSize)
{
	Initialize(iBufferSize);
}

int RingBuf::GetBufferSize(void)
{
	return totalSize;
}

int RingBuf::GetUseSize(void)
{
	return usedSize;
}

int RingBuf::GetFreeSize(void)
{
	return (totalSize - usedSize);
}

int RingBuf::Enqueue(char* chpData, int iSize)
{
	//�� ���� �뷮�� ������ ������
	if (GetFreeSize() == 0)
		return 0;

	//�־�� �ϴ� ũ�⺸�� ���� ������ ������ ���� ���� ������
	if (GetFreeSize() < iSize)
		return 0;

	//�б� / ����
	if (readPos <= writePos)
	{
		int rearSize = totalSize - writePos;

		//�� �� �ִ� �������� ����ϴ� �����Ͱ� ���� ���
		if (rearSize < iSize)
		{
			//�޺κ� �� �� �ִ� ���� �ϴ� ä���
			memcpy((dataBuf + writePos), chpData, rearSize);

			int frontSize = readPos;
			int remainSize = iSize - rearSize;

			//������ �ͼ� �տ��� ä�� �� ������ ä���
			//�տ� ������ �˳��ؼ� �� ä�� �� �ִ� ���
			if (remainSize <= frontSize)
			{
				memcpy(dataBuf, (chpData + rearSize), remainSize);

				writePos = remainSize;
				usedSize += iSize;

				return iSize;
			}
			//�տ��� ������ �����ؼ� ��ä��� ���
			else
			{
				memcpy(dataBuf, (chpData + rearSize), frontSize);

				writePos = frontSize;
				usedSize += (frontSize + rearSize);

				return (frontSize + rearSize);
			}
		}
		//�� �� �ִ� ������ ����ϴ� �����ͺ��� ���� ���
		else
		{
			memcpy((dataBuf + writePos), chpData, iSize);

			writePos = (writePos + iSize) % totalSize;
			usedSize += iSize;

			return iSize;
		}
	}
	//���� / �б�
	else
	{
		int remainSize = readPos - writePos;

		if (remainSize < iSize)
		{
			memcpy((dataBuf + writePos), chpData, remainSize);

			writePos = (writePos + remainSize) % totalSize;
			usedSize += remainSize;

			return remainSize;
		}
		else
		{
			memcpy((dataBuf + writePos), chpData, iSize);

			writePos = (writePos + iSize) % totalSize;
			usedSize += iSize;

			return iSize;
		}
	}

	return 0;
}

int RingBuf::Dequeue(char* chpData, int iSize)
{
	//�� �� �� ������ ������
	if (usedSize == 0)
		return 0;

	//�ִ� ũ�⺸�� �� ���� �޶�� �ϸ� ������ 
	if (usedSize < iSize)
		return 0;

	//�б� / ����
	if (readPos < writePos)
	{
		//�� �� �ִ� ũ��
		int remainSize = writePos - readPos;

		//���� ũ�⺸�� �� ���� ����� �� ���
		if (remainSize < iSize)
		{
			memcpy(chpData, (dataBuf + readPos), remainSize);

			readPos = (readPos + remainSize) % totalSize;
			usedSize -= remainSize;

			return remainSize;
		}
		//��û�� ��ŭ �� �� �� �ִ� ���
		else
		{
			memcpy(chpData, (dataBuf + readPos), iSize);

			readPos = (readPos + iSize) % totalSize;
			usedSize -= iSize;

			return iSize;
		}
	}
	//���� ���⼭ ���ٰ� ������ ���� ���۰� �� �� ��� ��
	else if (readPos >= writePos)
	{
		int rearSize = totalSize - readPos;

		if (rearSize < iSize)
		{
			memcpy(chpData, (dataBuf + readPos), rearSize);

			int frontSize = writePos;
			int remainSize = iSize - rearSize;

			if (remainSize <= frontSize)
			{
				memcpy((chpData + rearSize), dataBuf, remainSize);

				readPos = remainSize;
				usedSize -= iSize;

				return iSize;
			}
			else
			{
				memcpy((chpData + rearSize), dataBuf, frontSize);

				readPos = frontSize;
				usedSize -= (frontSize + rearSize);

				return (frontSize + rearSize);
			}
		}
		else
		{
			memcpy(chpData, (dataBuf + readPos), iSize);

			readPos = (readPos + iSize) % totalSize;
			usedSize -= iSize;

			return iSize;
		}
	}

	return 0;
}

int RingBuf::Peek(char* chpData, int iSize)
{
	if (usedSize == 0)
		return 0;

	if (readPos < writePos)
	{
		int remainSize = writePos - readPos;

		if (remainSize < iSize)
		{
			memcpy(chpData, (dataBuf + readPos), remainSize);

			return remainSize;
		}
		else
		{
			memcpy(chpData, (dataBuf + readPos), iSize);

			return iSize;
		}
	}
	//���� ���⼭ ���ٰ� ������ ���� ���۰� �� �� ��� ��
	else if (readPos >= writePos)
	{
		int rearSize = totalSize - readPos;

		if (rearSize < iSize)
		{
			memcpy(chpData, (dataBuf + readPos), rearSize);

			int frontSize = writePos;
			int remainSize = iSize - rearSize;

			if (remainSize <= frontSize)
			{
				memcpy((chpData + rearSize), dataBuf, remainSize);

				return iSize;
			}
			else
			{
				memcpy((chpData + rearSize), dataBuf, frontSize);

				return (frontSize + rearSize);
			}
		}
		else
		{
			memcpy(chpData, (dataBuf + readPos), iSize);

			return iSize;
		}
	}

	return 0;
}

int RingBuf::MoveRear(int iSize)
{
	int freeSize = GetFreeSize();

	if (iSize < freeSize)
	{
		writePos = (writePos + iSize) % totalSize;
		usedSize += iSize;

		return iSize;
	}
	else
	{
		writePos = (writePos + freeSize) % totalSize;
		usedSize += freeSize;

		return freeSize;
	}

	return 0;
}

int RingBuf::MoveFront(int iSize)
{
	int useSize = GetUseSize();

	if (iSize < useSize)
	{
		readPos = (readPos + iSize) % totalSize;
		usedSize -= iSize;

		return iSize;
	}
	else
	{
		readPos = (readPos + useSize) % totalSize;
		usedSize -= useSize;

		return useSize;
	}

	return 0;
}

void RingBuf::ClearBuffer(void)
{
	readPos = 0;
	writePos = 0;
	usedSize = 0;
}

char* RingBuf::GetFrontBufferPtr(void)
{
	return (dataBuf + readPos);
}

char* RingBuf::GetRearBufferPtr(void)
{
	return (dataBuf + writePos);
}

char* RingBuf::GetBufferPtr(void)
{
	return dataBuf;
}

unsigned int RingBuf::DirectEnqueueSize(void)
{
	if (GetFreeSize() == 0)
		return 0;

	if (readPos <= writePos)
		return (totalSize - writePos);
	else
		return (readPos - writePos);
}

unsigned int RingBuf::DirectDequeueSize(void)
{
	int _readPos = readPos;
	int _writePos = writePos;
	int _usedSize = usedSize;

	if (_usedSize == 0)
		return 0;

	if (_readPos < _writePos)
		return (_writePos - _readPos);
	else
		return (totalSize - _readPos);
}

void RingBuf::Lock()
{
	EnterCriticalSection(&cs);
}

void RingBuf::Unlock()
{
	LeaveCriticalSection(&cs);
}

void RingBuf::PrintBufState(void)
{
	printf("[Buffer State] R:%4d / W:%4d / T:%4d / U:%4d / F:%4d\n",
		readPos, writePos, totalSize, usedSize, GetFreeSize());
}
