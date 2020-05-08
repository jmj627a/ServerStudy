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

	//���� ������ < ������ ������
	if (_data < _ptr->data)
	{
		popnode = popNode(_ptr, _ptr->left, _data);
		return popnode;
	}
	// ������ ������ < ���� ������
	else if (_ptr->data < _data)
	{
		popnode = popNode(_ptr, _ptr->right, _data);
		return popnode;
	}
	//�����ؾ��� ������ �߰�
	else
	{
		//�� ��� �ڸ��� ��ü�� �����͸� �������� ������ ����, �� �����Ͱ� ���� �ִ� �����͸� ����

		//������ ������ left�� �ִ°�� -> left������ ��� right�� Ÿ�� leaf��带 ������
		if (_ptr->left != nullptr)
		{
			NODE* parnentnode = nullptr;
			NODE* changenode = findMinMax(parnentnode, _ptr->left);

			_ptr->data = changenode->data;

			parnentnode->right = nullptr;

			//������ �ٲ�ġ���ϰ� ������ �����ϱ�����, ���ʿ� ���� ��尡 �־��ٸ� ������ ��� �θ�� �̾��ֱ�
			if(changenode->left != nullptr)
				parnentnode->right = changenode->left;

			return changenode;
		}
		//������ �༮�� left�� ���� right�� �ִ°�� -> �� �θ�� �� right�� �̾��ְ� ���� ����
		else if (_ptr->left == nullptr && _ptr->right != nullptr)
		{
			//���� �θ��� ������ �ڽ��̸�
			if (_ParentPtr->right == _ptr)
				_ParentPtr->right = _ptr->right;
			
			//���� �θ��� ���� �ڽ��̸�
			else
				_ParentPtr->left = _ptr->left;

			return _ptr;
		}
		//������ �༮�� left right�� ��� ���� ��� 
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
		//�� ���� ��尡 leaf���. ���� �θ𿩾��� 
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
