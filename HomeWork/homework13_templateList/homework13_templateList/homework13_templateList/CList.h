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
			//인자로 들어온 Node 포인터를 저장
			_node = node;
		}

		//전위증가
		iterator&  operator++()
		{
			_node = _node->_Next;
			return *this;
		}

		//후위증가
		iterator operator ++(int)
		{
			iterator temp = *this;
			//현재 노드를 다음 노드로 이동
			_node = _node->_Next;
			return temp;
		}

		//const &로 해야 외부에서 값 변경을 하지 않음
		const T& operator *()
		{
			//현재 노드의 데이터를 뽑음
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
		//첫번째 노드를 가리키는 이터레이터 리턴
		return iterator(_head);
	}
	iterator end()
	{
		//Tail 노드를 가리키는(데이터가 없는 진짜 끝 노드) 이터레이터를 리턴
		//또는 끝으로 인지할 수 있는 이터레이터를 리턴
		return iterator(_tail);
	}
	
	iterator erase(iterator& eraseIter)
	{
		//- iterator의 그 노드를 지움.
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
	
		//- 그리고 지운 노드의 다음 노드를 가리키는 iterator 리턴
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
