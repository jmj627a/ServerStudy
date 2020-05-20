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
	//트리에 아무것도 없는 경우 그게 루트
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

	//트리 밸런스 맞추기
	makeBalanceTree(temp);

	//루트는 항상 BLACK
	m_pRoot->Color = BLACK;
}


stNODE* RBTree::pushNode(stNODE* _ptr, int _data)
{
	stNODE* newNode = nullptr;

	//들어온 데이터 < 포인터 데이터
	if (_data < _ptr->iData)
	{
		if (_ptr->pLeft == nullptr)
		{
			newNode = new stNODE(_data);
			_ptr->pLeft = newNode;
			//새로 추가된 노드는 무조건 red
			//새로 삽입된 노드는 무조건 끝단에 들어감
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
	// 포인터 데이터 < 들어온 데이터
	else if (_ptr->iData < _data)
	{
		if (_ptr->pRight == nullptr)
		{
			newNode = new stNODE(_data);
			_ptr->pRight = newNode;
			//새로 추가된 노드는 무조건 red
			//새로 삽입된 노드는 무조건 끝단에 들어감
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

	//들어온 데이터 < 포인터 데이터
	if (_data < _ptr->iData)
	{
		popnode = popNode(_ptr, _ptr->pLeft, _data);
		return popnode;
	}
	// 포인터 데이터 < 들어온 데이터
	else if (_ptr->iData < _data)
	{
		popnode = popNode(_ptr, _ptr->pRight, _data);
		return popnode;
	}
	//삭제해야할 데이터 발견
	else
	{
		//이 노드 자리를 대체할 데이터를 대입으로 가지고 오고, 그 데이터가 원래 있던 포인터를 리턴

		//삭제될 숫자의 left가 있는경우 -> left노드부터 계속 right를 타고 leaf노드를 가져옴
		if (_ptr->pLeft != nullptr)
		{
			stNODE* parnentnode = nullptr;
			stNODE* changenode = findMinMax(parnentnode, _ptr->pLeft);

			_ptr->iData = changenode->iData;

			parnentnode->pRight = nullptr;

			//데이터 바꿔치기하고 원본을 삭제하기전에, 왼쪽에 딸린 노드가 있었다면 삭제될 노드 부모랑 이어주기
			if (changenode->pLeft != nullptr)
				parnentnode->pRight = changenode->pLeft;

			return changenode;
		}
		//삭제될 녀석의 left가 없고 right만 있는경우 -> 내 부모와 내 right를 이어주고 나는 삭제
		else if (_ptr->pLeft == nullptr && _ptr->pRight != nullptr)
		{
			//내가 부모의 오른쪽 자식이면
			if (_ParentPtr->pRight == _ptr)
				_ParentPtr->pRight = _ptr->pRight;

			//내가 부모의 왼쪽 자식이면
			else
				_ParentPtr->pLeft = _ptr->pLeft;

			return _ptr;
		}
		//삭제될 녀석의 left right가 모두 없는 경우 
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
	//newNode를 기준으로 밸런싱

	//새로 들어온게 루트면 return
	if (newNode == m_pRoot)
		return;

	//부모가 BLACK이면 아무 문제 없이 return
	if (newNode->pParent->Color == BLACK)
		return;


	//부모는 무조건 RED인 상태
	//분기는 부모의 형제(uncle)를 확인하면서 나뉜다. -> 새로 추가된 내쪽과 삼촌쪽의 블랙의 개수를 맞추기 위해
	//내 부모가 할아버지의 왼편
	if (newNode->pParent->pParent->pLeft == newNode->pParent)
	{
		stNODE* pUncle = newNode->pParent->pParent->pRight;
		stNODE* pParent = newNode->pParent;
		stNODE* pGrand = newNode->pParent->pParent;

		//삼촌이 RED
		if (pUncle->Color == RED)
		{
			pParent->Color = BLACK;
			pUncle->Color = BLACK;
			pGrand->Color = RED;

			makeBalanceTree(pGrand);
		}
		//삼촌이 BLACK
		else if(pUncle->Color == BLACK)
		{
			//내가 부모의 오른자식
			if (newNode == pParent->pRight)
			{
				newNode = turnLeft(pParent);
			}

			//내가 부모의 왼자식
			pParent = newNode->pParent; //위에서 부모자식 관계가 바뀌었을수도 있으니 다시 설정
			pUncle = newNode->pParent->pParent->pRight;
			pGrand = newNode->pParent->pParent;

			pParent->Color = BLACK;
			pGrand->Color = RED;

			//할아버지 기준으로 우회전
			turnRight(pGrand);
		}
	}
	//내 부모가 할아버지의 오른편
	else
	{
		stNODE* pUncle = newNode->pParent->pParent->pLeft;
		stNODE* pParent = newNode->pParent;
		stNODE* pGrand = newNode->pParent->pParent;

		//삼촌이 RED
		if (pUncle->Color == RED)
		{
			pParent->Color = BLACK;
			pUncle->Color = BLACK;
			pGrand->Color = RED;

			makeBalanceTree(pGrand);
		}
		//삼촌이 BLACK
		else if (pUncle->Color == BLACK)
		{
			//내가 부모의 오른자식
			if (newNode == pParent->pLeft)
			{
				newNode = turnRight(pParent);
			}

			//내가 부모의 왼자식
			pParent = newNode->pParent; //위에서 부모자식 관계가 바뀌었을수도 있으니 다시 설정
			pUncle = newNode->pParent->pParent->pLeft;
			pGrand = newNode->pParent->pParent;

			pParent->Color = BLACK;
			pGrand->Color = RED;

			//할아버지 기준으로 우회전
			turnLeft(pGrand);
		}
	}
}

//기준이 되는 부모노드가 넘어옴
stNODE* RBTree::turnLeft(stNODE* _node)
{
	//_node를 기준으로 left 회전
	stNODE* pRightChild = _node->pRight;
	
	//부모 노드의 오른쪽 자식에, 그 오른쪽 자식 노드의 왼쪽 자식 노드를 넣는다. -> 자식이 옆으로 튕겨나가는 상황
	_node->pRight = pRightChild->pLeft;

	//근데 그렇게 옆으로 튕겨져 옮겨간 자식이 진짜 데이터가 있으면 -> 부모를 내 부모로 바꿔준다.
	if (pRightChild->pLeft != &Nil)
		pRightChild->pLeft->pParent = _node;

	//내 부모를 부모의 부모로 바꾼다. -> 부모가 내 자식으로 내려가니까.
	pRightChild->pParent = _node->pParent;

	//부모 == 할아버지의 왼편 위치
	if (_node == _node->pParent->pLeft)
		_node->pParent->pLeft = pRightChild; //할아버지의 왼자식을 나로 연결
	else //할아버지의 오른편 위치
		_node->pParent->pRight = pRightChild; //할아버지의 오른자식을 나로 연결

	//내 왼쪽에 부모 연결
	pRightChild->pLeft = _node;
	_node->pParent = pRightChild;

	//만약 원래 내 부모가 루트였다면 -> 이제 루트는 나
	if (pRightChild->pParent == &Nil)
		m_pRoot = pRightChild; 
	
	m_pRoot->Color = BLACK;

	return _node;
}

stNODE* RBTree::turnRight(stNODE* _node)
{
	//_node를 기준으로 right 회전
	
	stNODE* pLeftChild = _node->pLeft;

	//왼쪽 자식 노드의 오른쪽 자식 노드를 부모 노드의 왼쪽 자식으로 등록한다.
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
		//내 다음 노드가 leaf노드. 내가 부모여야함 
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
