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
	//Ʈ���� �ƹ��͵� ���� ��� �װ� ��Ʈ
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
	
	//���� ������ < ������ ������
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
	// ������ ������ < ���� ������
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

	return false;
}

void CBSearchTree::Print_Mid()
{
	if (m_pRoot == nullptr)
		return;

	print_left(m_pRoot->left);
	print_this(m_pRoot);
	print_right(m_pRoot->right);
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
