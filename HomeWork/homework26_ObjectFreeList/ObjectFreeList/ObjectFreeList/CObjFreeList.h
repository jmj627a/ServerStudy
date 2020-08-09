
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
#include <stdlib.h>


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
	CObjFreeList(int iBlockNum, bool bPlacementNew = false);
	virtual	~CObjFreeList();

	// �� �ϳ��� �Ҵ�޴´�.  
	// Parameters: ����.
	// Return: (DATA *) ����Ÿ �� ������.
	DATA* Alloc(void);

	// ������̴� ���� �����Ѵ�.
	// Parameters: (DATA *) �� ������.
	// Return: (BOOL) TRUE, FALSE.
	bool Free(DATA* pData);


	// ���� Ȯ�� �� �� ������ ��´�. (�޸�Ǯ ������ ��ü ����)
	// Parameters: ����.
	// Return: (int) �޸� Ǯ ���� ��ü ����
	int		GetAllocCount(void);

	// ���� ������� �� ������ ��´�.
	// Parameters: ����.
	// Return: (int) ������� �� ����.
	int		GetUseCount(void);


public:
	st_BLOCK_NODE* _pFreeNode = NULL;  //���� ������� ��ȯ�� (�̻��) ������Ʈ ���� ����.

	bool m_bPlacementNew;	//allocȣ�� �� ������/freeȣ�� �� �ı��� ȣ�� ����
	int m_iBlockNum;		//�ʱ� �� ����
	int m_iAllocCount;		//�޸� Ǯ ���� ����
	int m_iUseCount;		//������� �� ����
};






template <class DATA>
CObjFreeList<DATA>::CObjFreeList(int iBlockNum, bool bPlacementNew)
{
	_pFreeNode = NULL;
	m_iBlockNum = iBlockNum;
	m_bPlacementNew = bPlacementNew;
	m_iAllocCount = 0;
	m_iUseCount = 0;


	st_BLOCK_NODE* newNode;

	//������ ������ �Ҹ��ڸ� ȣ���ϰڴ� -> alloc free���� ����/�Ҹ��ڸ� ȣ���ҰŴ� ó���� ������ ȣ������ �ʾƾ� ��
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
	//������ ������ �Ҹ��ڸ� ȣ������ �ʰڴ� -> ó�� 1���� new�� �����ڸ� �ҷ��� ��. alloc Free���� ����/�Ҹ��� ȣ������ �ʰڴٴ� ��.
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
CObjFreeList<DATA>::~CObjFreeList()
{
	st_BLOCK_NODE* ptr = _pFreeNode;
	st_BLOCK_NODE* temp;

	//������ �Ҹ��ڸ� ȣ���ϰڴ� -> Free���� �Ҹ��ڸ� ȣ���ҰŴ� ���� �Ҹ��ڿ��� ȣ������ �ʾƾ� �� (ȣ���ϸ� 2�� ȣ���� ��)
	if (m_bPlacementNew)
	{
		while (ptr != NULL)
		{
			temp = ptr;
			ptr = ptr->stpNextBlock;
			free(temp);	//malloc���� �Ҵ��Ѱ� free�� ����. �Ҹ��� ȣ������ ����
		}
	}
	//������ �Ҹ��ڸ� ȣ������ �ʰڴ� -> Free���� �Ҹ��ڸ� ȣ������ �ʾ����� ���� ���������� �Ҹ��ڸ� ȣ���ؾ� ��.
	else
	{
		while (ptr != NULL)
		{
			temp = ptr;
			ptr = ptr->stpNextBlock;
			delete temp; //�Ҹ��� ȣ��
		}
	}
}

template <class DATA>
DATA* CObjFreeList<DATA>::Alloc(void)
{
	st_BLOCK_NODE* newNode;

	//����Ʈ�� ���������
	if (_pFreeNode == NULL)
	{
		newNode = new st_BLOCK_NODE;
		m_iAllocCount++;	//�Ҵ��� ũ�� ����
	}
	else
	{
		newNode = _pFreeNode;
		_pFreeNode = _pFreeNode->stpNextBlock;

		//������ ȣ��
		if (m_bPlacementNew)
			new(&newNode->data) DATA();
	}

	m_iUseCount++;			//������� �� ����

	return &newNode->data;

}

template <class DATA>
bool CObjFreeList<DATA>::Free(DATA* pData)
{
	if (m_iUseCount == 0)
		return true;

	if (pData == NULL)
		return false;

	//st_BLOCK_NODE* ��ŭ �տ� stpNextBlock����
	st_BLOCK_NODE* freeNode = (st_BLOCK_NODE*)((char*)pData - sizeof(st_BLOCK_NODE*));

	//���� ����� ���� ��� ��ġ�� pFreeNode�� �ִ´�. ���� �տ� �ִ� ���� ��
	freeNode->stpNextBlock = _pFreeNode;

	//�׸��� ���� ������ �� ����� ��ġ�� pFreeNode�� �˷���
	_pFreeNode = freeNode;

	m_iUseCount--;

	//�Ҹ��ڸ� �θ�
	if (m_bPlacementNew)
		freeNode->data.~DATA();

	return true;
}

template <class DATA>
int	CObjFreeList<DATA>::GetAllocCount(void) { return m_iAllocCount; }

template <class DATA>
int	CObjFreeList<DATA>::GetUseCount(void) { return m_iUseCount; }

#endif
