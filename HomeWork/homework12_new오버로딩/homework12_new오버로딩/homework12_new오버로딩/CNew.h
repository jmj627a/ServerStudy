#pragma once



class CNew
{
public:
	void* operator new( size_t size, const char* File, int Line);
	void* operator new[](size_t size, const char* File, int Line);

	// 다음 delete 는 실제로 쓰진 않지만 문법상 컴파일 오류를 막기 위해 만듬
	// 속은 비워둠.
	void operator delete (void* p, char* File, int Line);
	void operator delete[](void* p, char* File, int Line);

		// 실제로 사용할 delete
	void operator delete (void* p);
	void operator delete[](void* p);
};

