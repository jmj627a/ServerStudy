#pragma once

#include <list>


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

template <class DATA>
class CMemoryPool
{
private:
#pragma pack(push, 1)
	struct st_BLOCK_NODE
	{
		st_BLOCK_NODE()
		{
			stpNextBlock = NULL;
		}

		st_BLOCK_NODE* stpNextBlock;
		DATA data;
	};
#pragma pack(pop)
public:
	// 생성자, 파괴자.
	// Parameters:	(int) 초기 블럭 개수.
	//				(bool) Alloc 시 생성자 / Free 시 파괴자 호출 여부
	CMemoryPool(int blockSize = 10000, bool bPlacementNew = false);
	virtual ~CMemoryPool();

	// 블럭 하나를 할당받는다.  
	// Parameters: 없음.
	// Return: (DATA *) 데이타 블럭 포인터.
	DATA* Alloc(void);

	// 사용중이던 블럭을 해제한다.
	// Parameters: (DATA *) 블럭 포인터.
	// Return: (BOOL) TRUE, FALSE.
	bool Free(DATA* pData); 

	// 현재 확보 된 블럭 개수를 얻는다. (메모리풀 내부의 전체 개수)
	// Parameters: 없음.
	// Return: (int) 메모리 풀 내부 전체 개수
	int GetAllocCount(void);

	// 현재 사용중인 블럭 개수를 얻는다.
	// Parameters: 없음.
	// Return: (int) 사용중인 블럭 개수.
	int GetUseCount(void);

private:
	//스택 방식으로 반환된 (미사용) 오브젝트 블럭을 관리.
	st_BLOCK_NODE* _pFreeNode = NULL;  

	bool m_bPlacementNew;	//alloc호출 시 생성자/free호출 시 파괴자 호출 여부
	int m_iBlockNum;		//초기 블럭 개수
	int m_iAllocCount;		//메모리 풀 내부 개수
	int m_iUseCount;		//사용중인 블럭 개수

	CRITICAL_SECTION cs;
};



template <class DATA>
CMemoryPool<DATA>::CMemoryPool(int blockSize, bool bPlacementNew)	
	:m_iBlockNum(blockSize), m_bPlacementNew(bPlacementNew)
{
	InitializeCriticalSection(&cs);

	_pFreeNode = NULL;
	//m_iBlockNum = iBlockNum;
	//m_bPlacementNew = bPlacementNew;
	m_iAllocCount = 0;
	m_iUseCount = 0;

	st_BLOCK_NODE* newNode;

	//앞으로 생성자 소멸자를 호출하겠다 -> alloc free에서 생성/소멸자를 호출할거니 처음에 생성자 호출하지 않아야 함
	if (m_bPlacementNew)
	{
		for (int i = 0; i < m_iBlockNum; ++i)
		{
			newNode = (st_BLOCK_NODE*)malloc(sizeof(st_BLOCK_NODE));
			newNode->stpNextBlock = _pFreeNode;
			_pFreeNode = newNode;

			m_iAllocCount++;
		}
	}
	//앞으로 생성자 소멸자를 호출하지 않겠다 -> 처음 1번은 new로 생성자를 불러야 함. alloc Free에서 생성/소멸자 호출하지 않겠다는 것.
	else
	{
		for (int i = 0; i < m_iBlockNum; ++i)
		{
			newNode = new st_BLOCK_NODE();
			newNode->stpNextBlock = _pFreeNode;
			_pFreeNode = newNode;

			m_iAllocCount++;
		}
	}
}

template <class DATA>
CMemoryPool<DATA>::~CMemoryPool()
{
	DeleteCriticalSection(&cs);

	st_BLOCK_NODE* ptr = _pFreeNode;
	st_BLOCK_NODE* temp;

	//생성자 소멸자를 호출하겠다 -> Free에서 소멸자를 호출할거니 여기 소멸자에서 호출하지 않아야 함 (호출하면 2번 호출이 됨)
	if (m_bPlacementNew)
	{
		while (ptr != NULL)
		{
			temp = ptr;
			ptr = ptr->stpNextBlock;
			free(temp);	//malloc으로 할당한거 free로 해제. 소멸자 호출하지 않음
		}
	}
	//생성자 소멸자를 호출하지 않겠다 -> Free에서 소멸자를 호출하지 않았으니 여기 마지막에서 소멸자를 호출해야 함.
	else
	{
		while (ptr != NULL)
		{
			temp = ptr;
			ptr = ptr->stpNextBlock;
			delete temp; //소멸자 호출
		}
	}
}

template <class DATA>
DATA* CMemoryPool<DATA>::Alloc(void)
{
	st_BLOCK_NODE* newNode;

	//리스트가 비어있으면
	if (_pFreeNode == NULL)
	{
		newNode = new st_BLOCK_NODE;
		m_iAllocCount++;	//할당한 크기 증가
	}
	else
	{
		newNode = _pFreeNode;
		_pFreeNode = _pFreeNode->stpNextBlock;

		//생성자 호출
		if (m_bPlacementNew)
			new(&newNode->data) DATA();
	}

	m_iUseCount++;			//사용중인 블럭 증가

	LeaveCriticalSection(&cs);
	return &newNode->data;
}

template <class DATA>
bool CMemoryPool<DATA>::Free(DATA* pData)
{
	if (m_iUseCount == 0)
		return true;

	if (pData == NULL)
		return false;
	
	EnterCriticalSection(&cs);

	//st_BLOCK_NODE* 만큼 앞에 stpNextBlock존재
	st_BLOCK_NODE* freeNode = (st_BLOCK_NODE*)((char*)pData - sizeof(st_BLOCK_NODE*));

	//지금 노드의 다음 노드 위치에 pFreeNode에 넣는다. 가장 앞에 있는 블럭이 됨
	freeNode->stpNextBlock = _pFreeNode;

	//그리고 가장 앞으로 온 노드의 위치를 pFreeNode에 알려줌
	_pFreeNode = freeNode;

	m_iUseCount--;

	//소멸자를 부름
	if (m_bPlacementNew)
		freeNode->data.~DATA();

	LeaveCriticalSection(&cs);

	return true;
}

template <class DATA>
int CMemoryPool<DATA>::GetAllocCount(void)
{
	return m_iAllocCount;
}

template <class DATA>
int CMemoryPool<DATA>::GetUseCount(void)
{
	return m_iUseCount;
}