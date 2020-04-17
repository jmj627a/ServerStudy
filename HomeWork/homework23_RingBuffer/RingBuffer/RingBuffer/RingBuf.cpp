#include "RingBuf.h"


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


int RingBuf::GetUseSize(void)
{
	int num = (writePos - readPos + BUFFER_SIZE) % BUFFER_SIZE;
	return num;
}

int RingBuf::Enqueue(char* chpData, int iSize)
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

int RingBuf::Dequeue(char* &chpData, int iSize)
{
	int count = 0;
	
	int pos = readPos;

	while (iSize != 0)
	{
		//배열이 비어있음
		if (pos == writePos)
		{
			//printf("배열이 비어있습니다 \n");
			chpData = arr + readPos;
			readPos = pos;
 			return count;
		}

		pos = (pos + 1) % BUFFER_SIZE;
		//arr[pos] = NULL; //나중에 없앨 코드. 그냥 위치만 바꾸면 됨.
		count++;
		iSize--;
	}

	chpData = arr + readPos;
	readPos = pos;
	return count;
}

int RingBuf::Peek(char* chpData, int iSize)
{
	int count = 0;

	int pos = readPos;

	while (iSize == 0)
	{
		//배열이 비어있음
		if (pos == writePos)
		{
			//printf("배열이 비어있습니다 \n");
			chpData = arr + readPos;
			return count;
		}

		pos = (pos + 1) % BUFFER_SIZE;
		count++;
		iSize--;
	}

	chpData = arr + readPos;
	return count;
}