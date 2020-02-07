#pragma once
#include <stdio.h>

template <typename T>
class CList
{
public:
	struct Node
	{
		T _data;
		Node* _Prev;
		Node* _Next;
	};

	class iterator
	{
	private:
		friend class CList;
		Node* _node;
	public:
		iterator(Node* node = nullptr)
		{
			//���ڷ� ���� Node �����͸� ����
			_node = node;
		}

		//��������
		iterator&  operator++()
		{
			_node = _node->_Next;
			return *this;
		}

		//��������
		iterator operator ++(int)
		{
			iterator temp = *this;
			//���� ��带 ���� ���� �̵�
			_node = _node->_Next;
			return temp;
		}

		//const &�� �ؾ� �ܺο��� �� ������ ���� ����
		const T& operator *()
		{
			//���� ����� �����͸� ����
			return _node->_data;
		}

		bool operator !=(const iterator& iter)
		{
			return (_node != iter._node);
		}

		
	};

public:
	CList()
	{
		_head = new Node();
		_tail = new Node();

		_head->_data = 0;
		_head->_Prev = nullptr;
		_head->_Next = _tail;

		_tail->_data = 0;
		_tail->_Prev = _head;
		_tail->_Next = nullptr;
	}

	~CList()
	{
		clear();
	}


	iterator begin()
	{
		//ù��° ��带 ����Ű�� ���ͷ����� ����
		return iterator(_head);
	}
	iterator end()
	{
		//Tail ��带 ����Ű��(�����Ͱ� ���� ��¥ �� ���) ���ͷ����͸� ����
		//�Ǵ� ������ ������ �� �ִ� ���ͷ����͸� ����
		return iterator(_tail);
	}
	
	iterator erase(iterator& eraseIter)
	{
		//- iterator�� �� ��带 ����.
		iterator temp;
		Node*  node = NULL;

		temp._node = NULL;

		if (eraseIter._node == _head)
			return ++eraseIter;
		if (eraseIter._node == _tail)
			return ++eraseIter;

		node = eraseIter._node;
		node->_Next->_Prev = node->_Prev;
		node->_Prev->_Next = node->_Next;
		temp._node = node->_Next;
		delete node;
	
		//- �׸��� ���� ����� ���� ��带 ����Ű�� iterator ����
		return temp;

	}

	void push_front(T data)
	{
		Node* newNode = new Node();
		newNode->_data = data;
		newNode->_Next = _head->_Next;
		newNode->_Prev = _head;
		_head->_Next->_Prev = newNode;
		_head->_Next = newNode;
	}

	void push_back(T data)
	{
		Node* newNode = new Node();
		newNode->_data = data;
		newNode->_Next = _tail;
		newNode->_Prev = _tail->_Prev;
		_tail->_Prev->_Next = newNode;
		_tail->_Prev = newNode;
	}

	void clear()
	{
		CList<T>::iterator iter;

		for (iter = begin(); iter != end(); )
		{
			iter = erase(iter);
		}
	}

	int size() { return _size; };

	bool isEmpty() 
	{
		if (_head->_Next == _tail)
			return true;
		else
			return false;
	};

	void printAll()
	{
		CList<T>::iterator iter;
		for (iter = begin(); iter != end(); )
		{
			printf("%d\n", iter._node->_data);
			iter = ++iter;
		}
	}

	
private:
	int _size = 0;
	Node* _head;
	Node* _tail;
	
};
