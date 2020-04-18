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
		//�迭�� ������ �� ��
		if ((writePos + 1) % BUFFER_SIZE == readPos)
		{
			//printf("�迭�� �� á���ϴ� \n");
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

	//������ �� ����������
	while (iSize != 0)
	{
		//������ ���� �迭�� �������
		if (pos == writePos)
		{
			//���⼭ ������ �ɰ����� ��Ȳ�� ����. (readPos�� �迭 ������ �������� �Ѿ����)
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
		//�迭�� �������
		if (pos == writePos)
		{
			//printf("�迭�� ����ֽ��ϴ� \n");
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