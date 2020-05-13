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

    stNODE Nil; // 끝 리프노드. 무조건 블랙 / 데이터 무 / NULL 의 같은 용도.
    stNODE* m_pRoot;

public:
    RBTree();
    ~RBTree();


    //===========삽입==============
public:
    stNODE* InsertNode(int _data);

private:
    //insertNode에서 노드 삽입용 재귀함수로 호출
    stNODE* pushNode(stNODE* _ptr, int _data);


    //===========삭제==============
public:
    bool deleteNode(int _data);

private:
    //deleteNode에서 노드 삭제용 재귀함수로 호출
    stNODE* popNode(stNODE* _ParentPtr, stNODE* _ptr, int _data);
    //popNode에서 나를 대체할, 나보다 작은 애중에 제일 큰 애의 포인터를 구하는 함수
    stNODE* findMinMax(stNODE*& _ParentPtr, stNODE* _ptr);



    //===========밸런스==============
public:
    
    //newNode를 기준으로 밸런싱
    void makeBalanceTree(stNODE* newNode);
    
    //왼쪽으로 회전
    stNODE* turnLeft(stNODE* newNode);
    //오른쪽으로 회전
    stNODE* turnRight(stNODE* newNode);





    //===========출력==============
    void draw_Node(HDC hdc, stNODE* pNode, int _depth, int _xPoint, int _dir);



    void Print_Mid(); //중위순회

    void print_this(stNODE* _ptr);
    void print_left(stNODE* _ptr);
    void print_right(stNODE* _ptr);

};

