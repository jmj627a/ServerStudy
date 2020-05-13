#pragma once

enum NODE_COLOR
{
    BLACK = 0,
    RED
};

struct stNODE
{
    struct stNODE* pParent;
    struct stNODE* pLeft;
    struct stNODE* pRight;

    NODE_COLOR Color;

    int iData;


    stNODE(int _data)
    {
        iData = _data;
        pLeft = nullptr;
        pRight = nullptr;
    }

    stNODE() {}
};

class RBTree
{
public:

    stNODE Nil; // �� �������. ������ �� / ������ �� / NULL �� ���� �뵵.
    stNODE* m_pRoot;

public:
    RBTree();
    ~RBTree();


    //===========����==============
public:
    stNODE* InsertNode(int _data);

private:
    //insertNode���� ��� ���Կ� ����Լ��� ȣ��
    stNODE* pushNode(stNODE* _ptr, int _data);


    //===========����==============
public:
    bool deleteNode(int _data);

private:
    //deleteNode���� ��� ������ ����Լ��� ȣ��
    stNODE* popNode(stNODE* _ParentPtr, stNODE* _ptr, int _data);
    //popNode���� ���� ��ü��, ������ ���� ���߿� ���� ū ���� �����͸� ���ϴ� �Լ�
    stNODE* findMinMax(stNODE*& _ParentPtr, stNODE* _ptr);



    //===========�뷱��==============
public:
    
    //newNode�� �������� �뷱��
    void makeBalanceTree(stNODE* newNode);
    
    //�������� ȸ��
    stNODE* turnLeft(stNODE* newNode);
    //���������� ȸ��
    stNODE* turnRight(stNODE* newNode);





    //===========���==============
    void draw_Node(HDC hdc, stNODE* pNode, int _depth, int _xPoint, int _dir);



    void Print_Mid(); //������ȸ

    void print_this(stNODE* _ptr);
    void print_left(stNODE* _ptr);
    void print_right(stNODE* _ptr);

};

