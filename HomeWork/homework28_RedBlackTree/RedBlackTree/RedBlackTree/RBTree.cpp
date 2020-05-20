#include "RBTree.h"
#include <stdio.h>

RBTree::RBTree()
{
    Nil.Color = BLACK;
    Nil.pParent = nullptr;
    Nil.pLeft = nullptr;
    Nil.pRight = nullptr;

    m_pRoot = &Nil;
}

RBTree::~RBTree()
{

}

stNODE* RBTree::InsertNode(int _data)
{
	//Ʈ���� �ƹ��͵� ���� ��� �װ� ��Ʈ
	if (m_pRoot == nullptr)
	{
		stNODE* newNode = new stNODE(_data);
		m_pRoot = newNode;
		m_pRoot->Color = BLACK;
		m_pRoot->pLeft = &Nil;
		m_pRoot->pRight = &Nil;
		m_pRoot->pParent = nullptr;
		return newNode;
	}

	stNODE* temp = pushNode(m_pRoot, _data);

	//Ʈ�� �뷱�� ���߱�
	makeBalanceTree(temp);

	//��Ʈ�� �׻� BLACK
	m_pRoot->Color = BLACK;
}


stNODE* RBTree::pushNode(stNODE* _ptr, int _data)
{
	stNODE* newNode = nullptr;

	//���� ������ < ������ ������
	if (_data < _ptr->iData)
	{
		if (_ptr->pLeft == nullptr)
		{
			newNode = new stNODE(_data);
			_ptr->pLeft = newNode;
			//���� �߰��� ���� ������ red
			//���� ���Ե� ���� ������ ���ܿ� ��
			newNode->Color = RED;
			newNode->pLeft = &Nil;
			newNode->pRight = &Nil;
			newNode->pParent = _ptr;
			return newNode;
		}
		else
		{
			newNode = pushNode(_ptr->pLeft, _data);
		}
	}
	// ������ ������ < ���� ������
	else if (_ptr->iData < _data)
	{
		if (_ptr->pRight == nullptr)
		{
			newNode = new stNODE(_data);
			_ptr->pRight = newNode;
			//���� �߰��� ���� ������ red
			//���� ���Ե� ���� ������ ���ܿ� ��
			newNode->Color = RED;
			newNode->pLeft = &Nil;
			newNode->pRight = &Nil;
			newNode->pParent = _ptr;
			return newNode;
		}
		else
		{
			newNode = pushNode(_ptr->pRight, _data);
		}
	}

	return newNode;
}

bool RBTree::deleteNode(int _data)
{
	stNODE* ptr = m_pRoot;

	if (ptr == nullptr)
		return false;

	stNODE* deletenode = popNode(NULL, m_pRoot, _data);

	delete deletenode;

	return true;
}

stNODE* RBTree::popNode(stNODE* _ParentPtr, stNODE* _ptr, int _data)
{
	if (_ptr == nullptr)
		return nullptr;

	stNODE* popnode = nullptr;

	//���� ������ < ������ ������
	if (_data < _ptr->iData)
	{
		popnode = popNode(_ptr, _ptr->pLeft, _data);
		return popnode;
	}
	// ������ ������ < ���� ������
	else if (_ptr->iData < _data)
	{
		popnode = popNode(_ptr, _ptr->pRight, _data);
		return popnode;
	}
	//�����ؾ��� ������ �߰�
	else
	{
		//�� ��� �ڸ��� ��ü�� �����͸� �������� ������ ����, �� �����Ͱ� ���� �ִ� �����͸� ����

		//������ ������ left�� �ִ°�� -> left������ ��� right�� Ÿ�� leaf��带 ������
		if (_ptr->pLeft != nullptr)
		{
			stNODE* parnentnode = nullptr;
			stNODE* changenode = findMinMax(parnentnode, _ptr->pLeft);

			_ptr->iData = changenode->iData;

			parnentnode->pRight = nullptr;

			//������ �ٲ�ġ���ϰ� ������ �����ϱ�����, ���ʿ� ���� ��尡 �־��ٸ� ������ ��� �θ�� �̾��ֱ�
			if (changenode->pLeft != nullptr)
				parnentnode->pRight = changenode->pLeft;

			return changenode;
		}
		//������ �༮�� left�� ���� right�� �ִ°�� -> �� �θ�� �� right�� �̾��ְ� ���� ����
		else if (_ptr->pLeft == nullptr && _ptr->pRight != nullptr)
		{
			//���� �θ��� ������ �ڽ��̸�
			if (_ParentPtr->pRight == _ptr)
				_ParentPtr->pRight = _ptr->pRight;

			//���� �θ��� ���� �ڽ��̸�
			else
				_ParentPtr->pLeft = _ptr->pLeft;

			return _ptr;
		}
		//������ �༮�� left right�� ��� ���� ��� 
		else
		{
			if (_ParentPtr->pRight == _ptr)
				_ParentPtr->pRight = nullptr;
			else
				_ParentPtr->pLeft = nullptr;

			return _ptr;
		}
	}



}

void RBTree::makeBalanceTree(stNODE* newNode)
{
	//newNode�� �������� �뷱��

	//���� ���°� ��Ʈ�� return
	if (newNode == m_pRoot)
		return;

	//�θ� BLACK�̸� �ƹ� ���� ���� return
	if (newNode->pParent->Color == BLACK)
		return;


	//�θ�� ������ RED�� ����
	//�б�� �θ��� ����(uncle)�� Ȯ���ϸ鼭 ������. -> ���� �߰��� ���ʰ� �������� ���� ������ ���߱� ����
	//�� �θ� �Ҿƹ����� ����
	if (newNode->pParent->pParent->pLeft == newNode->pParent)
	{
		stNODE* pUncle = newNode->pParent->pParent->pRight;
		stNODE* pParent = newNode->pParent;
		stNODE* pGrand = newNode->pParent->pParent;

		//������ RED
		if (pUncle->Color == RED)
		{
			pParent->Color = BLACK;
			pUncle->Color = BLACK;
			pGrand->Color = RED;

			makeBalanceTree(pGrand);
		}
		//������ BLACK
		else if(pUncle->Color == BLACK)
		{
			//���� �θ��� �����ڽ�
			if (newNode == pParent->pRight)
			{
				newNode = turnLeft(pParent);
			}

			//���� �θ��� ���ڽ�
			pParent = newNode->pParent; //������ �θ��ڽ� ���谡 �ٲ�������� ������ �ٽ� ����
			pUncle = newNode->pParent->pParent->pRight;
			pGrand = newNode->pParent->pParent;

			pParent->Color = BLACK;
			pGrand->Color = RED;

			//�Ҿƹ��� �������� ��ȸ��
			turnRight(pGrand);
		}
	}
	//�� �θ� �Ҿƹ����� ������
	else
	{
		stNODE* pUncle = newNode->pParent->pParent->pLeft;
		stNODE* pParent = newNode->pParent;
		stNODE* pGrand = newNode->pParent->pParent;

		//������ RED
		if (pUncle->Color == RED)
		{
			pParent->Color = BLACK;
			pUncle->Color = BLACK;
			pGrand->Color = RED;

			makeBalanceTree(pGrand);
		}
		//������ BLACK
		else if (pUncle->Color == BLACK)
		{
			//���� �θ��� �����ڽ�
			if (newNode == pParent->pLeft)
			{
				newNode = turnRight(pParent);
			}

			//���� �θ��� ���ڽ�
			pParent = newNode->pParent; //������ �θ��ڽ� ���谡 �ٲ�������� ������ �ٽ� ����
			pUncle = newNode->pParent->pParent->pLeft;
			pGrand = newNode->pParent->pParent;

			pParent->Color = BLACK;
			pGrand->Color = RED;

			//�Ҿƹ��� �������� ��ȸ��
			turnLeft(pGrand);
		}
	}
}

//������ �Ǵ� �θ��尡 �Ѿ��
stNODE* RBTree::turnLeft(stNODE* _node)
{
	//_node�� �������� left ȸ��
	stNODE* pRightChild = _node->pRight;
	
	//�θ� ����� ������ �ڽĿ�, �� ������ �ڽ� ����� ���� �ڽ� ��带 �ִ´�. -> �ڽ��� ������ ƨ�ܳ����� ��Ȳ
	_node->pRight = pRightChild->pLeft;

	//�ٵ� �׷��� ������ ƨ���� �Űܰ� �ڽ��� ��¥ �����Ͱ� ������ -> �θ� �� �θ�� �ٲ��ش�.
	if (pRightChild->pLeft != &Nil)
		pRightChild->pLeft->pParent = _node;

	//�� �θ� �θ��� �θ�� �ٲ۴�. -> �θ� �� �ڽ����� �������ϱ�.
	pRightChild->pParent = _node->pParent;

	//�θ� == �Ҿƹ����� ���� ��ġ
	if (_node == _node->pParent->pLeft)
		_node->pParent->pLeft = pRightChild; //�Ҿƹ����� ���ڽ��� ���� ����
	else //�Ҿƹ����� ������ ��ġ
		_node->pParent->pRight = pRightChild; //�Ҿƹ����� �����ڽ��� ���� ����

	//�� ���ʿ� �θ� ����
	pRightChild->pLeft = _node;
	_node->pParent = pRightChild;

	//���� ���� �� �θ� ��Ʈ���ٸ� -> ���� ��Ʈ�� ��
	if (pRightChild->pParent == &Nil)
		m_pRoot = pRightChild; 
	
	m_pRoot->Color = BLACK;

	return _node;
}

stNODE* RBTree::turnRight(stNODE* _node)
{
	//_node�� �������� right ȸ��
	
	stNODE* pLeftChild = _node->pLeft;

	//���� �ڽ� ����� ������ �ڽ� ��带 �θ� ����� ���� �ڽ����� ����Ѵ�.
	_node->pLeft = pLeftChild->pRight;

	if (pLeftChild->pRight != &Nil)
		pLeftChild->pRight->pParent = _node;

	pLeftChild->pParent = _node->pParent;

	if (_node == _node->pParent->pLeft)
		_node->pParent->pLeft = pLeftChild;
	else
		_node->pParent->pRight = pLeftChild;

	pLeftChild->pRight = _node;
	_node->pParent = pLeftChild;

	if (pLeftChild->pParent == &Nil)
		m_pRoot = pLeftChild;
	
	m_pRoot->Color = BLACK;


	return _node;
}

stNODE* RBTree:: findMinMax(stNODE*& _ParentPtr, stNODE* _ptr)
{
	stNODE* ptr = _ptr;

	while (ptr->pRight != nullptr)
	{
		//�� ���� ��尡 leaf���. ���� �θ𿩾��� 
		if (ptr->pRight->pRight == nullptr)
			_ParentPtr = ptr;

		ptr = ptr->pRight;
	}


	return ptr;
}

void RBTree::Print_Mid()
{
	if (m_pRoot == nullptr)
		return;

	print_left(m_pRoot->pLeft);
	print_this(m_pRoot);
	print_right(m_pRoot->pRight);

	printf("\n");
}

void RBTree::print_this(stNODE* _ptr)
{
	printf(" %d ", _ptr->iData);
}

void RBTree::print_left(stNODE* _ptr)
{
	if (_ptr == nullptr)
		return;
	print_left(_ptr->pLeft);
	print_this(_ptr);
	print_right(_ptr->pRight);
}

void RBTree::print_right(stNODE* _ptr)
{
	if (_ptr == nullptr)
		return;
	print_left(_ptr->pLeft);
	print_this(_ptr);
	print_right(_ptr->pRight);
}
