#include "pch.h"
#include "RBTree.h"

RBTree::RBTree()
{
    Nil.Color = BLACK;
    Nil.pParent = &Nil;
    Nil.pLeft = &Nil;
    Nil.pRight = &Nil;

    m_pRoot = &Nil;
}

RBTree::~RBTree()
{

}

stNODE* RBTree::InsertNode(int _data)
{
	//Ʈ���� �ƹ��͵� ���� ��� �װ� ��Ʈ
	if (m_pRoot == &Nil)
	{
		stNODE* newNode = new stNODE(_data);
		m_pRoot = newNode;
		m_pRoot->Color = BLACK;
		m_pRoot->pLeft = &Nil;
		m_pRoot->pRight = &Nil;
		m_pRoot->pParent = &Nil;
		return newNode;
	}

	stNODE* temp = pushNode(m_pRoot, _data);

	//�ߺ������Ͷ� �߰��� ��������� nil
	if (temp != &Nil)
	{
		//Ʈ�� �뷱�� ���߱�
		makeBalanceTree_insert(temp);
	}

	//��Ʈ�� �׻� BLACK
	m_pRoot->Color = BLACK;

	return temp;
}


stNODE* RBTree::pushNode(stNODE* _ptr, int _data)
{
	stNODE* newNode = &Nil;

	//���� ������ < ������ ������
	if (_data < _ptr->iData)
	{
		if (_ptr->pLeft == &Nil)
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
		if (_ptr->pRight == &Nil)
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
	//�ߺ�������
	else
	{
		return &Nil;
	}

	return newNode;
}

bool RBTree::deleteNode(int _data)
{
	stNODE* ptr = m_pRoot;

	if (ptr == &Nil)
		return false;

	stNODE* deletenode = popNode(NULL, m_pRoot, _data);

	if (deletenode != &Nil)
	{
		/*makeBalanceTree_delete(deletenode);*/

		if (deletenode->pParent->pLeft == deletenode)
			deletenode->pParent->pLeft = &Nil;
		else if (deletenode->pParent->pRight == deletenode)
			deletenode->pParent->pRight = &Nil;

		delete deletenode;
	}

	//��Ʈ�� �׻� BLACK
	m_pRoot->Color = BLACK;
	return true;
}

void RBTree::makeBalanceTree_delete(stNODE* deletenode)
{
	//������ ��� RED -> �θ�� �̹� BLACK�̾����� �뷱���� ������ ����.
	if (deletenode->Color == RED) 
	{
		deletenode->Color = BLACK;
	}
	//������ ��� BLACK -> BLACK��带 �����ؼ� �� ������ Ʋ�����Ƿ� �뷱�� �۾� �ʿ� + �θ��ڽ��� ���巹���� ��� �۾� �ʿ�
	else
	{
		//������ ��尡 �θ��� ���ʿ� ��ġ
		if (deletenode->pParent->pLeft == deletenode)
		{
			stNODE* pSibling = deletenode->pParent->pRight;
			//���� ����� �θ�� �ڽ��� ��� ������ ���
			if (deletenode->pParent->Color == RED && deletenode->pLeft->Color == RED)
			{
				//���巹���� ������ �ڽ��� black���� �ٲٸ鼭 �ذ�
				deletenode->pLeft->Color = BLACK;
			}
			else if (deletenode->pParent->Color == RED && deletenode->pRight->Color == RED)
			{
				//���巹���� ������ �ڽ��� black���� �ٲٸ鼭 �ذ�
				deletenode->pRight->Color = BLACK;
			}
			//���� ����� ������ RED
			else if (pSibling->Color == RED)
			{
				pSibling->Color = BLACK;
				stNODE* temp = turnLeft(deletenode->pParent);
				temp->Color = RED;

				makeBalanceTree_delete(deletenode->pLeft);
			}
			//���� ����� ������ ���̰� ������ ���� �ڽ��� ��
			else if (pSibling->Color == BLACK && pSibling->pLeft->Color == BLACK && pSibling->pRight->Color == BLACK)
			{
				pSibling->Color = RED;

				//�츮 �θ��� ���� -1�Ǿ�����, �θ� �������� �ٽ� �뷱��
				makeBalanceTree_delete(deletenode->pParent);
			}
			else if(pSibling->Color == BLACK &&
				(pSibling->pLeft->Color ==RED) || (pSibling->pRight->Color == RED))
			{
				//��������� ������ ���̰� ������ ���ڽ��� ����
				if (pSibling->pLeft->Color == RED)
				{
					pSibling->pLeft->Color = BLACK;
					pSibling->Color = RED;
					turnRight(pSibling);
				}

				//���� ����� ������ ���̰� ������ �����ڽ��� ����
				pSibling->Color = pSibling->pParent->Color;
				pSibling->pParent->Color = BLACK;
				pSibling->pRight->Color = BLACK;
				turnLeft(pSibling->pParent);
			}
		}
		//������ ��尡 �θ��� �����ʿ� ��ġ
		else if (deletenode->pParent->pRight == deletenode)
		{
			stNODE* pSibling = deletenode->pParent->pLeft;
			//���� ����� �θ�� �ڽ��� ��� ������ ���
			if (deletenode->pParent->Color == RED && deletenode->pRight->Color == RED)
			{
				//���巹���� ������ �ڽ��� black���� �ٲٸ鼭 �ذ�
				deletenode->pRight->Color = BLACK;
			}
			else if (deletenode->pParent->Color == RED && deletenode->pLeft->Color == RED)
			{
				//���巹���� ������ �ڽ��� black���� �ٲٸ鼭 �ذ�
				deletenode->pLeft->Color = BLACK;
			}
			//���� ����� ������ RED
			else if (pSibling->Color == RED)
			{
				pSibling->Color = BLACK;
				stNODE* temp = turnRight(deletenode->pParent);
				temp->Color = RED;

				makeBalanceTree_delete(deletenode->pRight);
			}
			//���� ����� ������ ���̰� ������ ���� �ڽ��� ��
			else if (pSibling->Color == BLACK && pSibling->pLeft->Color == BLACK && pSibling->pRight->Color == BLACK)
			{
				pSibling->Color = RED;

				//�츮 �θ��� ���� -1�Ǿ�����, �θ� �������� �ٽ� �뷱��
				makeBalanceTree_delete(deletenode->pParent);
			}
			else if (pSibling->Color == BLACK &&
				(pSibling->pLeft->Color == RED) || (pSibling->pRight->Color == RED))
			{
				//��������� ������ ���̰� ������ �����ڽ��� ����
				if (pSibling->pRight->Color == RED)
				{
					pSibling->pRight->Color = BLACK;
					pSibling->Color = RED;
					turnLeft(pSibling);
				}

				//���� ����� ������ ���̰� ������ ���ڽ��� ����
				pSibling = deletenode->pParent->pLeft;
				pSibling->Color = pSibling->pParent->Color;
				pSibling->pParent->Color = BLACK;
				pSibling->pLeft->Color = BLACK;
				turnRight(pSibling->pParent);
			}
		}
	}
}


stNODE* RBTree::popNode(stNODE* _ParentPtr, stNODE* _ptr, int _data)
{
	if (_ptr == &Nil)
		return &Nil;

	stNODE* popnode = &Nil;

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
		if (_ptr->pLeft != &Nil)
		{
			stNODE* parnentnode = _ptr->pParent;
			stNODE* changenode = findMinMax(parnentnode, _ptr->pLeft);

			makeBalanceTree_delete(changenode);

			_ptr->iData = changenode->iData;

			//??
			//parnentnode->pRight = &Nil;

			//������ �ٲ�ġ���ϰ� ������ �����ϱ�����, ���ʿ� ���� ��尡 �־��ٸ� ������ ��� �θ�� �̾��ֱ�
			if (changenode->pLeft != &Nil)
			{
				changenode->pParent->pLeft = changenode->pLeft;
				changenode->pLeft->pParent = changenode->pParent;
			}
			return changenode;
		}
		//������ �༮�� left�� ���� right�� �ִ°�� -> �� �θ�� �� right�� �̾��ְ� ���� ����
		else if (_ptr->pLeft == &Nil && _ptr->pRight != &Nil)
		{
			makeBalanceTree_delete(_ptr);

			//���� ��Ʈ�ΰ��
			if(_ptr==m_pRoot)
			{
				stNODE* parnentnode = _ptr->pParent;
				stNODE* changenode = findMaxMin(parnentnode, _ptr->pRight);
				_ptr->iData = changenode->iData;
				_ptr = changenode;
			}
			//���� �θ��� ������ �ڽ��̸�
			else if (_ParentPtr->pRight == _ptr)
				_ParentPtr->pRight = _ptr->pRight;

			//���� �θ��� ���� �ڽ��̸�
			else if(_ParentPtr->pLeft == _ptr)
				_ParentPtr->pLeft = _ptr->pRight;
			


			return _ptr;
		}
		//������ �༮�� left right�� ��� ���� ��� 
		else
		{
			makeBalanceTree_delete(_ptr);

			if (_ParentPtr->pRight == _ptr)
				_ParentPtr->pRight = &Nil;
			else
				_ParentPtr->pLeft = &Nil;

			return _ptr;
		}
	}
}

void RBTree::makeBalanceTree_insert(stNODE* newNode)
{
	//newNode�� �������� �뷱��

	//���� ���°� ��Ʈ�� return
	if (newNode == m_pRoot)
		return;

	//�θ� BLACK�̸� �ƹ� ���� ���� return
	if (newNode->pParent->Color == BLACK)
		return;

	stNODE* pUncle = newNode->pParent->pParent->pRight;
	stNODE* pParent = newNode->pParent;
	stNODE* pGrand = newNode->pParent->pParent;

	//�θ�� ������ RED�� ����
	//�б�� �θ��� ����(uncle)�� Ȯ���ϸ鼭 ������. -> ���� �߰��� ���ʰ� �������� ���� ������ ���߱� ����
	//�� �θ� �Ҿƹ����� ����
	if (newNode->pParent->pParent->pLeft == newNode->pParent)
	{
		pUncle = newNode->pParent->pParent->pRight;

		//������ RED
		if (pUncle->Color == RED)
		{
			pParent->Color = BLACK;
			pUncle->Color = BLACK;
			pGrand->Color = RED;

			makeBalanceTree_insert(pGrand);
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
		pUncle = newNode->pParent->pParent->pLeft;

		//������ RED
		if (pUncle->Color == RED)
		{
			pParent->Color = BLACK;
			pUncle->Color = BLACK;
			pGrand->Color = RED;

			makeBalanceTree_insert(pGrand);
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

//ȸ���� ������ �Ǵ� ��尡 �Ѿ��
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

	while (ptr->pRight != &Nil)
	{
		//�� ���� ��尡 leaf���. ���� �θ𿩾��� 
		if (ptr->pRight->pRight == &Nil)
			_ParentPtr = ptr;

		ptr = ptr->pRight;
	}


	return ptr;
}

stNODE* RBTree::findMaxMin(stNODE*& _ParentPtr, stNODE* _ptr)
{
	stNODE* ptr = _ptr;

	while (ptr->pLeft != &Nil)
	{
		//�� ���� ��尡 leaf���. ���� �θ𿩾��� 
		if (ptr->pLeft->pLeft == &Nil)
			_ParentPtr = ptr;

		ptr = ptr->pLeft;
	}


	return ptr;
}

void RBTree::draw_Node(HDC hdc, stNODE* pNode, int _depth, int _xPoint, int _dir)
{
	if (pNode == &Nil)	return;
	
	int Depth = _depth;

	TCHAR data[10];
	int xPoint = WIN_WIDTH / 2;

	if (pNode->Color == BLACK)
		SetTextColor(hdc, RGB(0, 0, 0));
	else if (pNode->Color == RED)
		SetTextColor(hdc, RGB(255, 0, 0));

	MoveToEx(hdc, _xPoint, (Depth - 1) * 80 + 20, NULL);

	if (pNode == m_pRoot)xPoint = WIN_WIDTH / 2;
	else if (_dir == 0)	 xPoint = _xPoint - WIN_WIDTH / pow(2.0, Depth);
	else if (_dir == 1)	 xPoint = _xPoint + WIN_WIDTH / pow(2.0, Depth);

	LineTo(hdc, xPoint, Depth * 80 - 10);

	wsprintf(data, TEXT(" %d "), pNode->iData);
	TextOut(hdc, xPoint-10, Depth * 80, data, wcslen(data));
	

	draw_Node(hdc, pNode->pLeft, Depth + 1, xPoint, 0);
	draw_Node(hdc, pNode->pRight, Depth + 1, xPoint, 1);
}

void RBTree::Print_Mid()
{
	if (m_pRoot == &Nil)
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
	if (_ptr == &Nil)
		return;
	print_left(_ptr->pLeft);
	print_this(_ptr);
	print_right(_ptr->pRight);
}

void RBTree::print_right(stNODE* _ptr)
{
	if (_ptr == &Nil)
		return;
	print_left(_ptr->pLeft);
	print_this(_ptr);
	print_right(_ptr->pRight);
}
