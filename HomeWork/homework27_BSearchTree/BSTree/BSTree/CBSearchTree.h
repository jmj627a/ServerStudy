#pragma once


#define LEFT	0
#define RIGHT	1

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

class CBSearchTree
{
private:
	
	NODE* m_pRoot;

public:
	CBSearchTree();
	~CBSearchTree();


public:
	bool InsertNode(int _data);
	bool deleteNode(int _data);
	
	
	
	
	void Print_Mid(); //중위순회

private:
	//insertNode에서 노드 삽입용 재귀함수로 호출
	bool pushNode(NODE* _ptr, int _data);

	//deleteNode에서 노드 삭제용 재귀함수로 호출
	NODE* popNode(NODE* _ParentPtr, NODE* _ptr, int _data);
	//popNode에서 나를 대체할, 나보다 작은 애중에 제일 큰 애의 포인터를 구하는 함수
	NODE* findMinMax(NODE* _ParentPtr, NODE* _ptr);

	void print_this(NODE* _ptr);
	void print_left(NODE* _ptr);
	void print_right(NODE* _ptr);
};

