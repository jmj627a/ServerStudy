#pragma once


#define LEFT	0
#define RIGHT	1

class CBSearchTree
{
private:

	struct NODE
	{
		NODE* left;		//내 데이터보다 작음 
		NODE* right;	//내 데이터보다 큼
		int data;

		NODE(int _data)
		{
			data = _data;
			left = nullptr;
			right = nullptr;
		}
	};

	NODE* m_pRoot;

public:
	CBSearchTree();
	~CBSearchTree();


public:
	bool InsertNode(int _data);
	void Print_Mid(); //중위순회

private:
	//insertNode에서 노드 삽입용 재귀함수로 호출
	bool pushNode(NODE* _ptr, int _data);

	void print_this(NODE* _ptr);
	void print_left(NODE* _ptr);
	void print_right(NODE* _ptr);
};

