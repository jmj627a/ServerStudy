#include "RingBuf.h"
#include <string>
#include <Windows.h>

void RingBuf::Initial(int iBufferSize)
{
}

RingBuf::RingBuf(void)
{
	readPos = 0;
	writePos = 0;
}

RingBuf::RingBuf(int iBufferSize)
{
	readPos = 0;
	writePos = 0;
}


int RingBuf::GetBufferSize(void)
{
	return BUFFER_SIZE;
}

int RingBuf::GetUseSize(void)
{
	int num = (writePos - readPos + BUFFER_SIZE) % BUFFER_SIZE;
	return num;
}

int RingBuf::GetFreeSize(void)
{
	int num = (writePos - readPos + BUFFER_SIZE) % BUFFER_SIZE;
	return BUFFER_SIZE - num;
}

void RingBuf::RemoveData(int iSize)
{
	if (BUFFER_SIZE < iSize)
		return;
	else
	{
		if (readPos + iSize >= BUFFER_SIZE)
			readPos = (readPos + iSize) % BUFFER_SIZE;
		else
			readPos += iSize;
	}
}

int RingBuf::Enqueue(char* chpData, int iSize)
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
				// iSize - pSize = 새로 0부터 넣어야 할 데이터 크기
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
			// 넣을수 있는 공간 >= 넣어야 하는 공간
			memcpy(arr + writePos, chpData, iSize);
			writePos += iSize;
			return iSize;
		}
	}
	else
	{
		// rear보다 front가 큰 경우
		int pSize = readPos - writePos;

		if (pSize < iSize)
		{
			// pSize만큼만 넣고 리턴한다.
			memcpy(arr + writePos, chpData, pSize);
			writePos += pSize;
			return pSize;
		}
		else
		{
			// iSize 만큼 다 넣는다.
			memcpy(arr + writePos, chpData, iSize);
			writePos += iSize;
			return iSize;
		}

	}
}

int RingBuf::Dequeue(char* chpData, int iSize)
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
		if (pSize < iSize)
		{
			memcpy(chpData, arr + readPos, pSize);
			getSize += pSize;

			if (iSize - pSize > writePos)
			{
				// iSize - pSize = 새로 0부터 넣어야 할 데이터 크기
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

int RingBuf::Peek(char* chpData, int iSize)
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
				// iSize - pSize = 새로 0부터 넣어야 할 데이터 크기
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

int RingBuf::MoveRear(int iSize)
{
	writePos = (writePos + iSize) % BUFFER_SIZE;
	return writePos;
}

int RingBuf::MoveFront(int iSize)
{
	readPos = (readPos + iSize) % BUFFER_SIZE;
	return readPos;
}

void RingBuf::ClearBuffer(void)
{
	//사실 0으로 지울 필요 없고, pos를 같게 위치시키면 데이터가 없다는 의미와 같음.
	memset(arr, 0, sizeof(arr));
	readPos = 0;
	writePos= 0;
}

char * RingBuf::GetFrontBufferPtr(void)
{
	return arr + readPos;
}

char * RingBuf::GetRearBufferPtr(void)
{
	return arr + writePos;
}

char * RingBuf::GetBufferPtr(void)
{
	return arr;
}

int RingBuf::DirectEnqueueSize(void)
{
	if (readPos <= writePos)
	{
		int endPointIndex = BUFFER_SIZE - 1;

		return endPointIndex - writePos;
	}
	else
		return readPos - writePos;
}

int RingBuf::DirectDequeueSize(void)
{
	if (readPos >= writePos)
	{
		int endPointIndex = BUFFER_SIZE - 1;

		return endPointIndex - readPos;
	}
	else
		return writePos - readPos;
}
