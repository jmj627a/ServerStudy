
/*---------------------------------------------------------------

	procademy MemoryPool.

	메모리 풀 클래스 (오브젝트 풀 / 프리리스트)
	특정 데이타(구조체,클래스,변수)를 일정량 할당 후 나눠쓴다.

	- 사용법.

	procademy::CMemoryPool<DATA> MemPool(300, FALSE);
	DATA *pData = MemPool.Alloc();

	pData 사용

	MemPool.Free(pData);


----------------------------------------------------------------*/
#ifndef  __PROCADEMY_MEMORY_POOL__
#define  __PROCADEMY_MEMORY_POOL__
#include <new.h>


template <class DATA>
class CObjFreeList
{
public:

	// 각 블럭 앞에 사용될 노드 구조체.
	struct st_BLOCK_NODE
	{
		st_BLOCK_NODE()
		{
			stpNextBlock = NULL;
		}

		st_BLOCK_NODE* stpNextBlock;
		DATA data;
	};




public:

	// 생성자, 파괴자.
	// Parameters:	(int) 초기 블럭 개수.
	//				(bool) Alloc 시 생성자 / Free 시 파괴자 호출 여부
	// Return:
	CMemoryPool(int iBlockNum, bool bPlacementNew = false)
	{
		_pFreeNode = NULL;
		m_iBlockNum = iBlockNum;
		m_bPlacementNew = bPlacementNew;
		m_iAllocCount = 0;
		m_iUseCount = 0;


		st_BLOCK_NODE* newNode;

		if (m_bPlacementNew)
		{
			for (int i = 0; i < m_iBlockNum; ++i)
			{
				newNode = (st_BLOCK_NODE*)malloc(sizeof(st_BLOCK_NODE));
				Free(&newNode->data);
			}
		}
		else
		{
			for (int i = 0; i < m_iBlockNum; ++i)
			{
				newNode = new st_BLOCK_NODE;
				Free(&newNode->data);
			}
		}


	}
	virtual	~CMemoryPool()
	{
		st_BLOCK_NODE* ptr = _pFreeNode;
		st_BLOCK_NODE* temp;

		if (m_bPlacementNew)
		{
			while (ptr != NULL)
			{
				temp = ptr;
				ptr = ptr->stpNextBlock;
				free(temp);	//소멸자 호출하지 않음
			}
		}
		else
		{
			while (ptr != NULL)
			{
				temp = ptr;
				ptr = ptr->stpNextBlock;
				delete temp; //마지지막에 한번에 소멸자 호출
			}
		}
	}


	// 블럭 하나를 할당받는다.  
	// Parameters: 없음.
	// Return: (DATA *) 데이타 블럭 포인터.
	DATA* Alloc(void)
	{
		st_BLOCK_NODE* newNode;

		//리스트가 비어있으면
		if (_pFreeNode == NULL)
		{
			newNode = new st_BLOCK_NODE;
		}
		else
		{
			newNode = _pFreeNode;
			_pFreeNode = _pFreeNode->stpNextBlock
		}

		m_iAllocCount++;

		return &newNode->data;

	}

	// 사용중이던 블럭을 해제한다.
	// Parameters: (DATA *) 블럭 포인터.
	// Return: (BOOL) TRUE, FALSE.
	bool Free(DATA* pData)
	{
		st_BLOCK_NODE* freeNode = (st_BLOCK_NODE*)((char*)pData - sizeof(st_BLOCK_NODE*));

		freeNode->stpNextBlock = _pFreeNode;

		_pFreeNode = freeNode;

		if (m_bPlacementNew) 
			freeNode->data.~DATA();

		return true;
	}


	// 현재 확보 된 블럭 개수를 얻는다. (메모리풀 내부의 전체 개수)
	// Parameters: 없음.
	// Return: (int) 메모리 풀 내부 전체 개수
	int		GetAllocCount(void) { return m_iAllocCount; }

	// 현재 사용중인 블럭 개수를 얻는다.
	// Parameters: 없음.
	// Return: (int) 사용중인 블럭 개수.
	int		GetUseCount(void) { return m_iUseCount; }



	st_BLOCK_NODE* _pFreeNode;  //스택 방식으로 반환된 (미사용) 오브젝트 블럭을 관리.

	bool m_bPlacementNew;	//alloc호출 시 생성자/free호출 시 파괴자 호출 여부
	int m_iBlockNum;		//초기 블럭 개수
	int m_iAllocCount;		//메모리 풀 내부 개수
	int m_iUseCount;		//사용중인 블럭 개수
};


#endif
