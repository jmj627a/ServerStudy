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
			//���ڷ� ���� Node �����͸� ����
			_node = node;
		}

		iterator operator ++(int)
		{
			//���� ��带 ���� ���� �̵�
			_node = _node->_Next;
		}

		int& operator *()
		{
			//���� ����� �����͸� ����
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
		//ù��° ��带 ����Ű�� ���ͷ����� ����
		return iterator(_head);
	}
	iterator end()
	{
		//Tail ��带 ����Ű��(�����Ͱ� ���� ��¥ �� ���) ���ͷ����͸� ����
		//�Ǵ� ������ ������ �� �ִ� ���ͷ����͸� ����
		return iterator(_tail);
	}

	void push_front(int data);
	void push_back(int data);
	void clear();
	int size() { return _size; };
	bool empty() { };

	iterator erase(iterator eraseIter)
	{
		//- iterator�� �� ��带 ����.
		iterator iter;

		for (iter = begin(); iter != end(); )
		{
			if (*iter == *eraseIter)
			{
				iter.
			}
		}
		
		//- �׸��� ���� ����� ���� ��带 ����Ű�� iterator ����
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

