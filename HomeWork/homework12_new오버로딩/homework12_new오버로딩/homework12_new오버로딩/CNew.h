#pragma once



class CNew
{
public:
	void* operator new( size_t size, const char* File, int Line);
	void* operator new[](size_t size, const char* File, int Line);

	// ���� delete �� ������ ���� ������ ������ ������ ������ ���� ���� ����
	// ���� �����.
	void operator delete (void* p, char* File, int Line);
	void operator delete[](void* p, char* File, int Line);

		// ������ ����� delete
	void operator delete (void* p);
	void operator delete[](void* p);
};

