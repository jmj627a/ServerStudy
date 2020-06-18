#include "stdafx.h"
#include "CRingBuffer.h"


void CRingBuffer::Initial(int iBufferSize)
{
}

CRingBuffer::CRingBuffer(void)
{
	readPos = 0;
	writePos = 0;
}

CRingBuffer::CRingBuffer(int iBufferSize)
{
	readPos = 0;
	writePos = 0;
}


int CRingBuffer::GetBufferSize(void)
{
	return BUFFER_SIZE;
}

int CRingBuffer::GetUseSize(void)
{
	int num = (writePos - readPos + BUFFER_SIZE) % BUFFER_SIZE;
	return num;
}

int CRingBuffer::GetFreeSize(void)
{
	int num = (writePos - readPos + BUFFER_SIZE) % BUFFER_SIZE;
	return BUFFER_SIZE - num;
}

int CRingBuffer::Enqueue(char* chpData, int iSize)
{
	int inputSize = 0;

	if (readPos <= writePos)
	{
		int pSize = DirectEnqueueSize();

		if (pSize < iSize)
		{
			memcpy(arr + writePos, chpData, pSize);
			chpData += pSize;
			inputSize += pSize;

			if (iSize - pSize > readPos)
			{
				// iSize - pSize = ���� 0���� �־�� �� ������ ũ��
				memcpy(arr, chpData, readPos);
				inputSize += readPos;
				writePos = readPos;
			}
			else
			{
				memcpy(arr, chpData, iSize - pSize);
				inputSize += iSize - pSize;
				writePos = iSize - pSize;
			}
			return inputSize;
		}
		else
		{
			// ������ �ִ� ���� >= �־�� �ϴ� ����
			memcpy(arr + writePos, chpData, iSize);
			writePos += iSize;
			return iSize;
		}
	}
	else
	{
		// rear���� front�� ū ���
		int pSize = readPos - writePos;

		if (pSize < iSize)
		{
			// pSize��ŭ�� �ְ� �����Ѵ�.
			memcpy(arr + writePos, chpData, pSize);
			writePos += pSize;
			return pSize;
		}
		else
		{
			// iSize ��ŭ �� �ִ´�.
			memcpy(arr + writePos, chpData, iSize);
			writePos += iSize;
			return iSize;
		}

	}
}

int CRingBuffer::Dequeue(char* chpData, int iSize)
{
	int getSize = 0;

	if (readPos <= writePos)
	{
		int pSize = writePos - readPos;

		if (pSize < iSize)
		{
			memcpy(chpData, arr + readPos, pSize);
			readPos += pSize;
			return pSize;
		}
		else
		{
			memcpy(chpData, arr + readPos, iSize);
			readPos += iSize;
			return iSize;
		}
	}
	else
	{
		int pSize = DirectDequeueSize();
		int frontIndex = readPos;
		if (pSize < iSize)
		{
			memcpy(chpData, arr + readPos, pSize);
			frontIndex += pSize;
			getSize += pSize;

			if (iSize - pSize > writePos)
			{
				// iSize - pSize = ���� 0���� �־�� �� ������ ũ��
				memcpy(chpData + pSize, arr, writePos);
				getSize += writePos;
				readPos = writePos;
			}
			else
			{
				memcpy(chpData + pSize, arr, iSize - pSize);
				getSize += iSize - pSize;
				readPos = iSize - pSize;
			}
			return getSize;
		}
		else
		{
			memcpy(chpData, arr + readPos, iSize);
			readPos += iSize;
			return iSize;
		}
	}
}

int CRingBuffer::Peek(char* chpData, int iSize)
{
	int getSize = 0;

	if (readPos <= writePos)
	{
		int pSize = writePos - readPos;

		if (pSize < iSize)
		{
			memcpy(chpData, arr + readPos, pSize);
			return pSize;
		}
		else
		{
			memcpy(chpData, arr + readPos, iSize);
			return iSize;
		}
	}
	else
	{
		int pSize = DirectDequeueSize();
		int frontIndex = readPos;
		if (pSize < iSize)
		{
			memcpy(chpData, arr + readPos, pSize);
			frontIndex += pSize;
			getSize += pSize;
			chpData += pSize;

			if (iSize - pSize > writePos)
			{
				// iSize - pSize = ���� 0���� �־�� �� ������ ũ��
				memcpy(chpData + pSize, arr, writePos);
				getSize += writePos;
			}
			else
			{
				memcpy(chpData + pSize, arr, iSize - pSize);
				getSize += iSize - pSize;
			}
			return getSize;
		}
		else
		{
			memcpy(chpData, arr + readPos, iSize);
			return iSize;
		}
	}
}

int CRingBuffer::MoveRear(int iSize)
{
	writePos = (writePos + iSize + BUFFER_SIZE) % BUFFER_SIZE;
	return writePos;
}

int CRingBuffer::MoveFront(int iSize)
{
	readPos = (readPos + iSize + BUFFER_SIZE) % BUFFER_SIZE;
	return readPos;
}

void CRingBuffer::ClearBuffer(void)
{
	memset(arr, 0, sizeof(arr));
	readPos = 0;
	writePos = 0;
}

char * CRingBuffer::GetFrontBufferPtr(void)
{
	return arr + readPos;
}

char * CRingBuffer::GetRearBufferPtr(void)
{
	return arr + writePos;
}

int CRingBuffer::DirectEnqueueSize(void)
{
	if (readPos <= writePos)
	{
		int endPointIndex = BUFFER_SIZE;// -2;

		return endPointIndex - writePos;
	}
	else
		return readPos - writePos;
}

int CRingBuffer::DirectDequeueSize(void)
{
	if (readPos >= writePos)
	{
		int endPointIndex = BUFFER_SIZE;// -2;

		return endPointIndex - readPos;
	}
	else
		return writePos - readPos;
}
