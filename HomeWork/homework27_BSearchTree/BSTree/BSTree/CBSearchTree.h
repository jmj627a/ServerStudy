#pragma once


#define LEFT	0
#define RIGHT	1

struct NODE
{
	NODE* left;		//�� �����ͺ��� ���� 
	NODE* right;	//�� �����ͺ��� ŭ
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
	
	
	
	
	void Print_Mid(); //������ȸ

private:
	//insertNode���� ��� ���Կ� ����Լ��� ȣ��
	bool pushNode(NODE* _ptr, int _data);

	//deleteNode���� ��� ������ ����Լ��� ȣ��
	NODE* popNode(NODE* _ParentPtr, NODE* _ptr, int _data);
	//popNode���� ���� ��ü��, ������ ���� ���߿� ���� ū ���� �����͸� ���ϴ� �Լ�
	NODE* findMinMax(NODE* _ParentPtr, NODE* _ptr);

	void print_this(NODE* _ptr);
	void print_left(NODE* _ptr);
	void print_right(NODE* _ptr);
};

