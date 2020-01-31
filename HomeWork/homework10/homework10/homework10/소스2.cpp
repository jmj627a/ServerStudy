#include<iostream> using namespace std; 

template <typename T> struct Node // 노드 구조체 
{ 
public: T value; 
	  struct Node<T> *next = nullptr; 
}; 

template <typename T> 
class SingleLinked_List //싱글 링크드 리스트 
{
private: 
	Node<T>* head;
	   //머리(front) 
	   Node<T>* tail;
	   //꼬리(back) 
	   int size = 0;
	   // 링크의 길이 
public:
	SingleLinked_List() : head(nullptr), tail(nullptr) {}
	// 생성자 리스트 
	~SingleLinked_List() {}
	//소멸자 
	void AddNode(T _value) // 노드 추가하기(back) 
	{
		Node<T>* node = new Node<T>; //노드 동적 할당 
		size++; node->value = _value;
		node->next = nullptr;
		if (head == nullptr) //만약 머리가 없을경우 
		{
			head = node;
			tail = node;
		}
		else // 만약 머리가 있을경우 뒤에 연결 
		{
			tail->next = node;
			tail = tail->next;
		}
	}
}