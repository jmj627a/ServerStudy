
/*---------------------------------------------------------------

	procademy MemoryPool.

	�޸� Ǯ Ŭ���� (������Ʈ Ǯ / ��������Ʈ)
	Ư�� ����Ÿ(����ü,Ŭ����,����)�� ������ �Ҵ� �� ��������.

	- ����.

	procademy::CMemoryPool<DATA> MemPool(300, FALSE);
	DATA *pData = MemPool.Alloc();

	pData ���

	MemPool.Free(pData);


----------------------------------------------------------------*/
#ifndef  __PROCADEMY_MEMORY_POOL__
#define  __PROCADEMY_MEMORY_POOL__
#include <new.h>


template <class DATA>
class CObjFreeList
{
public:

	// �� �� �տ� ���� ��� ����ü.
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

	// ������, �ı���.
	// Parameters:	(int) �ʱ� �� ����.
	//				(bool) Alloc �� ������ / Free �� �ı��� ȣ�� ����
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
				free(temp);	//�Ҹ��� ȣ������ ����
			}
		}
		else
		{
			while (ptr != NULL)
			{
				temp = ptr;
				ptr = ptr->stpNextBlock;
				delete temp; //���������� �ѹ��� �Ҹ��� ȣ��
			}
		}
	}


	// �� �ϳ��� �Ҵ�޴´�.  
	// Parameters: ����.
	// Return: (DATA *) ����Ÿ �� ������.
	DATA* Alloc(void)
	{
		st_BLOCK_NODE* newNode;

		//����Ʈ�� ���������
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

	// ������̴� ���� �����Ѵ�.
	// Parameters: (DATA *) �� ������.
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


	// ���� Ȯ�� �� �� ������ ��´�. (�޸�Ǯ ������ ��ü ����)
	// Parameters: ����.
	// Return: (int) �޸� Ǯ ���� ��ü ����
	int		GetAllocCount(void) { return m_iAllocCount; }

	// ���� ������� �� ������ ��´�.
	// Parameters: ����.
	// Return: (int) ������� �� ����.
	int		GetUseCount(void) { return m_iUseCount; }



	st_BLOCK_NODE* _pFreeNode;  //���� ������� ��ȯ�� (�̻��) ������Ʈ ���� ����.

	bool m_bPlacementNew;	//allocȣ�� �� ������/freeȣ�� �� �ı��� ȣ�� ����
	int m_iBlockNum;		//�ʱ� �� ����
	int m_iAllocCount;		//�޸� Ǯ ���� ����
	int m_iUseCount;		//������� �� ����
};


#endif
