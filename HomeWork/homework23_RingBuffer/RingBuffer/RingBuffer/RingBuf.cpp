#include "RingBuf.h"
#include <string>

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

int RingBuf::Dequeue(char* chpData, int iSize)
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
				memcpy(chpData + (count - afterCount), arr , afterCount);
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