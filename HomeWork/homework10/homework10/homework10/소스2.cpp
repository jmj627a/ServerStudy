#include<iostream> using namespace std; 

template <typename T> struct Node // ��� ����ü 
{ 
public: T value; 
	  struct Node<T> *next = nullptr; 
}; 

template <typename T> 
class SingleLinked_List //�̱� ��ũ�� ����Ʈ 
{
private: 
	Node<T>* head;
	   //�Ӹ�(front) 
	   Node<T>* tail;
	   //����(back) 
	   int size = 0;
	   // ��ũ�� ���� 
public:
	SingleLinked_List() : head(nullptr), tail(nullptr) {}
	// ������ ����Ʈ 
	~SingleLinked_List() {}
	//�Ҹ��� 
	void AddNode(T _value) // ��� �߰��ϱ�(back) 
	{
		Node<T>* node = new Node<T>; //��� ���� �Ҵ� 
		size++; node->value = _value;
		node->next = nullptr;
		if (head == nullptr) //���� �Ӹ��� ������� 
		{
			head = node;
			tail = node;
		}
		else // ���� �Ӹ��� ������� �ڿ� ���� 
		{
			tail->next = node;
			tail = tail->next;
		}
	}
}