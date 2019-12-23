#include <stdio.h>


int quickArr[10] = { 7,4,9,10,2,5,6,8,1,0 };
//int pivot = 0; //0번째 값이 피봇
//int right = 9; //배열 맨 마짐가 9번째 값이 right
//int left = 1; //피봇 다음 값이 left

int qSort(int left, int right)
{
	if (left >= right)
	{
		return 0;
	} 

	int pivot = left - 1;
	int first = left;
	int temp = quickArr[pivot];
	quickArr[pivot] = quickArr[left];
	quickArr[left] = temp;

	for (int i = left + 1; i <= right; i++)
	{
		if (quickArr[left] < quickArr[i])
		{
			first++;
			temp = quickArr[first];
			quickArr[first] = quickArr[i];
			quickArr[i] = temp;
		}
	}

	temp = quickArr[first];
	quickArr[first] = quickArr[left];
	quickArr[left] = temp;

	qSort(left, first - 1);
	qSort(first + 1, right);

}

void quickSort(int _left, int _right)
{
	if (_right <= _left)
		return;

	printf("( %d ~ %d )", _left, _right);
	for (int i = 0; i < 10; ++i)
		printf("%d ", quickArr[i]);
	printf("\n");

	int pivot = _left;

	int left = pivot + 1;
	int right = _right;

	while (true)
	{
		for (int i = left; i <= _right; ++i)
		{
			if (quickArr[i] > quickArr[pivot])
			{
				left = i;
				break;
			}
		}

		for (int i = right; i >= _left; --i)
		{
			if (quickArr[i] <= quickArr[pivot])
			{
				right = i;
				break;
			}
		}

		if (right <= left)
			break;

		int temp = quickArr[right];
		quickArr[right] = quickArr[left];
		quickArr[left] = temp;
	}
	
	int temp = quickArr[pivot];
	for (int i = _left; i < left-1 ; ++i)
	{
		quickArr[i] = quickArr[i + 1];
	}
	quickArr[right] = temp;


	quickSort(_left, right-1);
	quickSort(right + 1, _right);

}


void main()
{
	quickSort(0, 9);
}


//https://medium.com/@leeyh0216/%ED%80%B5%EC%A0%95%EB%A0%AC-3cb1e521e656