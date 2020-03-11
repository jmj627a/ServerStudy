#pragma once
#include <Windows.h>
#include "global.h"
#include "CQueue.h"

class CGraphWnd
{
public:

	// 본 클래스에는 싱글라인,멀티라인,숫자표기,파이그래프 등
	// 다양한 그래프 출력을 모두 내장하고 있다.

	// 생성시 타입만을 지정하여 해당 윈도우의 그래프를 지정 해줄 것임
	// 이 타입은 내부 프로시저의 WM_PAINT 에서 분기를 통해
	// 해당 타입에 맞는 랜더 함수로 들어가게 될 것임.

	enum TYPE
	{
		LINE_SINGLE,
		LINE_MULTI,
		NUMBER,
		ONOFF,
		PIE
	};


	// 윈도우 프로시저를 직접 만들어보면 알게 되겠지만...
	// WndProc 는 함수 포인터가 윈도우 생성시 전달 되어야 한다.
	// 클래스의 맴버 함수는 this 가 존재해야 하므로 콜백 함수로 등록이 불가능 함

	// 결국 WndProc 맴버 함수는 static 으로 선언 되어야 한다.
	// 문제는 static 인 WndProc 에는 this 가 존재하지 않기 때문에 맴버변수 접근이 불가능 하다.

	// 이를 위해서 결국 윈도우 마다의 특정 고유값을 기준으로 직접 this 를 찾아서 활용하는 방법뿐이 없다.
	// 그래서 윈도우 핸들인 hwnd 값과 해당 윈도우를 만들어준 객체의 포인터를 테이블로 매칭시켜 this 를 찾아내야만 한다.

	// HWND 와 해당 윈도우를 만든 객체의 포인터를 맵핑시키는 구조체.
	// 이 구조체를 역시 static 배열로 보관하며, WndProc 에서는 매번 검색하여 사용한다.

	// 이는 윈도우의 user data 와 extra data 활용으로 더 간단히 해결도 가능하지만
	// 일단 api 의 힘을 빌리지 않는 언어 차원의 해결방법으로 해보도록 한다.

	typedef struct ST_HWNDtoTHIS
	{
		HWND		hWnd;
		CGraphWnd	*pThis;

	} stHWNDtoTHIS;

public:

	CGraphWnd(HINSTANCE hInstance, HWND hWndParent, TYPE enType, int iPosX, int iPosY, int iWidth, int iHeight);
	~CGraphWnd();

	/////////////////////////////////////////////////////////
	// 윈도우 프로시저
	/////////////////////////////////////////////////////////
	static LRESULT CALLBACK WndChildProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	/////////////////////////////////////////////////////////
	// 데이터 넣기.
	/////////////////////////////////////////////////////////
	BOOL	InsertData(int iData);

	ATOM initChildClass(HINSTANCE hInstance);

protected:

	//------------------------------------------------------
	// 윈도우 핸들, this 포인터 매칭 테이블 관리.
	//------------------------------------------------------
	BOOL			PutThis(void);


	static CGraphWnd	*GetThis(HWND hWnd);
	static stHWNDtoTHIS 	HWNDTable[dfMAXCHILD];
	static int changeWindowPosition(int _y);

private:

	//------------------------------------------------------
	// 부모 윈도우 핸들, 내 윈도우 핸들, 인스턴스 핸들
	//------------------------------------------------------
	//------------------------------------------------------
	// 윈도우 위치,크기,색상, 그래프 타입 등.. 자료
	//------------------------------------------------------
	TYPE		_enGraphType;

	//------------------------------------------------------
	// 더블 버퍼링용 메모리 DC, 메모리 비트맵
	//------------------------------------------------------
	HDC		_hMemDC;
	HBITMAP		_hBitmap;


	//------------------------------------------------------
	// 데이터
	//------------------------------------------------------
	//데이터 큐
	static CQueue* queue;
};