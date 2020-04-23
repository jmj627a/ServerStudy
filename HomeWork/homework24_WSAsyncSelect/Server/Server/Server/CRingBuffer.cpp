#include "CRingBuffer.h"
#include "stdafx.h"

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
	int count = 0;

	while (iSize != 0)
	{
		//배열에 데이터 꽉 참
		if ((writePos + 1) % BUFFER_SIZE == readPos)
		{
			//printf("배열이 꽉 찼습니다 \n");
			return count;
		}

		arr[writePos] = *chpData;
		writePos = (writePos + 1) % BUFFER_SIZE;

		count++;
		chpData++;
		iSize--;

	}

	return count;
}

int CRingBuffer::Dequeue(char* chpData, int iSize)
{
	int count = 0;
	int pos = readPos;

	bool isOneCircle = false;
	int afterCount = 0;

	//끝까지 다 읽을때까지
	while (iSize != 0)
	{
		//읽으려 보니 배열이 비어있음
		if (pos == writePos)
		{
			//여기서 반으로 쪼개지는 상황이 있음. (readPos가 배열 끝에서 시작으로 넘어갔을때)
			if (isOneCircle == true)
			{
				memcpy(chpData, arr + readPos, count - afterCount);
				memcpy(chpData + (count - afterCount), arr, afterCount);
			}
			else
				memcpy(chpData, arr + readPos, count);


			readPos = pos;
			return count;
			
		}

		pos = (pos + 1) % BUFFER_SIZE;

		if (isOneCircle == true)
			afterCount++;

		if (pos == 0)
			isOneCircle = true;

		count++;
		iSize--;
	}

	if (isOneCircle == true)
	{
		memcpy(chpData, arr + readPos, count - afterCount);
		memcpy(chpData + (count - afterCount), arr, afterCount);
	}
	else
		memcpy(chpData, arr + readPos, count);

	readPos = pos;
	return count;
}

int CRingBuffer::Peek(char* chpData, int iSize)
{
	int count = 0;
	int pos = readPos;

	bool isOneCircle = false;
	int afterCount = 0;

	//끝까지 다 읽을때까지
	while (iSize != 0)
	{
		//읽으려 보니 배열이 비어있음
		if (pos == writePos)
		{
			//여기서 반으로 쪼개지는 상황이 있음. (readPos가 배열 끝에서 시작으로 넘어갔을때)
			if (isOneCircle == true)
			{
				memcpy(chpData, arr + readPos, count - afterCount);
				memcpy(chpData + (count - afterCount), arr, afterCount);
			}
			else
				memcpy(chpData, arr + readPos, count);

			return count;
		}

		pos = (pos + 1) % BUFFER_SIZE;

		if (isOneCircle == true)
			afterCount++;

		if (pos == 0)
			isOneCircle = true;

		count++;
		iSize--;
	}

	if (isOneCircle == true)
	{
		memcpy(chpData, arr + readPos, count - afterCount);
		memcpy(chpData + (count - afterCount), arr, afterCount);
	}
	else
		memcpy(chpData, arr + readPos, count);

	return count;
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
	if (readPos < writePos)
		return BUFFER_SIZE - writePos;
	else if (readPos > writePos)
		return readPos - writePos;
	else
		return BUFFER_SIZE - writePos;
}

int CRingBuffer::DirectDequeueSize(void)
{
	if (readPos < writePos)
		return writePos - readPos;
	else if (readPos > writePos)
		return BUFFER_SIZE - readPos;
	else
		return BUFFER_SIZE - readPos;
}
