#pragma once

struct Node;
class CList
{
public:
	struct Node
	{
		int _data;
		Node* _Prev;
		Node* _Next;

		void adf();
	};

	class iterator
	{
	private:
		Node* _node;
	public:
		iterator(Node* node = nullptr)
		{
			//인자로 들어온 Node 포인터를 저장
			_node = node;
		}

		iterator operator ++(int)
		{
			//현재 노드를 다음 노드로 이동
			_node = _node->_Next;
		}

		int& operator *()
		{
			//현재 노드의 데이터를 뽑음
			return _node->_data;
		}

		bool operator !=(const iterator& iter)
		{
			return (_node != iter._node);
		}

		/*int& getID()
		{
			return _node->_data;
		}*/
	};

public:
	CList();
	~CList();

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

	void push_front(int data);
	void push_back(int data);
	void clear();
	int size() { return _size; };
	bool empty() { };

	iterator erase(iterator eraseIter)
	{
		//- iterator의 그 노드를 지움.
		iterator iter;

		for (iter = begin(); iter != end(); )
		{
			if (*iter == *eraseIter)
			{
				iter.
			}
		}
		
		//- 그리고 지운 노드의 다음 노드를 가리키는 iterator 리턴
		return iter++;
	}
private:
	int _size = 0;
	Node* _head;
	Node* _tail;

	
};

CList::CList()
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

CList::~CList()
{
	//iterator iter = begin();
	//for (iter; iter != end(); )
	//{
	//
	//}
	//delete 
}

void CList::push_front(int data)
{
	Node* newNode = new Node();
	newNode->_data = data;
	newNode->_Next = _head->_Next;
	_head->_Next->_Prev = newNode;
	_head->_Next = newNode;
}

void CList::push_back(int data)
{
	Node* newNode = new Node();
	newNode->_data = data;
	newNode->_Next = _tail;
	newNode->_Prev = _tail->_Prev;
	_tail->_Prev->_Next = newNode;
	_tail->_Prev = newNode;
}

void CList::clear()
{
}

