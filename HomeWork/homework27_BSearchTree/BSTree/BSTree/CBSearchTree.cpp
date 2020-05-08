#include "CBSearchTree.h"
#include <stdio.h>


CBSearchTree::CBSearchTree()
{
	m_pRoot = nullptr;
}


CBSearchTree::~CBSearchTree()
{
}

bool CBSearchTree::InsertNode(int _data)
{
	//트리에 아무것도 없는 경우 그게 루트
	if (m_pRoot == nullptr)
	{
		NODE* newNode = new NODE(_data);
		m_pRoot = newNode;
		return true;
	}

	pushNode(m_pRoot, _data);
}


bool CBSearchTree::pushNode(NODE* _ptr, int _data)
{
	//들어온 데이터 < 포인터 데이터
	if (_data < _ptr->data)
	{
		if (_ptr->left == nullptr)
		{
			NODE* newNode = new NODE(_data);
			_ptr->left = newNode;
			return true;
		}
		else
		{
			pushNode(_ptr->left, _data);
		}
	}
	// 포인터 데이터 < 들어온 데이터
	else if (_ptr->data < _data)
	{
		if (_ptr->right == nullptr)
		{
			NODE* newNode = new NODE(_data);
			_ptr->right = newNode;
			return true;
		}
		else
		{
			pushNode(_ptr->right, _data);
		}
	}

	return true;
}

bool CBSearchTree::deleteNode(int _data)
{
	NODE* ptr = m_pRoot;

	if (ptr == nullptr)
		return false;

	NODE* deletenode = popNode(NULL, m_pRoot, _data);

	delete deletenode;

	return true;
}

NODE* CBSearchTree::popNode(NODE* _ParentPtr, NODE* _ptr, int _data)
{
	if (_ptr == nullptr)
		return nullptr;

	NODE* popnode = nullptr;

	//들어온 데이터 < 포인터 데이터
	if (_data < _ptr->data)
	{
		popnode = popNode(_ptr, _ptr->left, _data);
		return popnode;
	}
	// 포인터 데이터 < 들어온 데이터
	else if (_ptr->data < _data)
	{
		popnode = popNode(_ptr, _ptr->right, _data);
		return popnode;
	}
	//삭제해야할 데이터 발견
	else
	{
		//이 노드 자리를 대체할 데이터를 대입으로 가지고 오고, 그 데이터가 원래 있던 포인터를 리턴

		//삭제될 숫자의 left가 있는경우 -> left노드부터 계속 right를 타고 leaf노드를 가져옴
		if (_ptr->left != nullptr)
		{
			NODE* parnentnode = nullptr;
			NODE* changenode = findMinMax(parnentnode, _ptr->left);

			_ptr->data = changenode->data;

			parnentnode->right = nullptr;

			//데이터 바꿔치기하고 원본을 삭제하기전에, 왼쪽에 딸린 노드가 있었다면 삭제될 노드 부모랑 이어주기
			if(changenode->left != nullptr)
				parnentnode->right = changenode->left;

			return changenode;
		}
		//삭제될 녀석의 left가 없고 right만 있는경우 -> 내 부모와 내 right를 이어주고 나는 삭제
		else if (_ptr->left == nullptr && _ptr->right != nullptr)
		{
			//내가 부모의 오른쪽 자식이면
			if (_ParentPtr->right == _ptr)
				_ParentPtr->right = _ptr->right;
			
			//내가 부모의 왼쪽 자식이면
			else
				_ParentPtr->left = _ptr->left;

			return _ptr;
		}
		//삭제될 녀석의 left right가 모두 없는 경우 
		else
		{
			if (_ParentPtr->right == _ptr)
				_ParentPtr->right = nullptr;
			else
				_ParentPtr->left = nullptr;
			
			return _ptr;
		}
	}
}

NODE* CBSearchTree::findMinMax(NODE* &_ParentPtr, NODE* _ptr)
{
	NODE* ptr = _ptr;

	while (ptr->right != nullptr)
	{
		//내 다음 노드가 leaf노드. 내가 부모여야함 
		if (ptr->right->right == nullptr)
			_ParentPtr = ptr;

		ptr = ptr->right;
	}


	return ptr;
}

void CBSearchTree::Print_Mid()
{
	if (m_pRoot == nullptr)
		return;

	print_left(m_pRoot->left);
	print_this(m_pRoot);
	print_right(m_pRoot->right);

	printf("\n");
}

void CBSearchTree::print_this(NODE* _ptr)
{
	printf(" %d ", _ptr->data);
}

void CBSearchTree::print_left(NODE* _ptr)
{
	if (_ptr == nullptr)
		return;
	print_left(_ptr->left);
	print_this(_ptr);
	print_right(_ptr->right);
}

void CBSearchTree::print_right(NODE* _ptr)
{
	if (_ptr == nullptr)
		return;
	print_left(_ptr->left);
	print_this(_ptr);
	print_right(_ptr->right);
}
