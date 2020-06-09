#pragma once
#include <Windows.h>

#ifndef  __PACKET__
#define  __PACKET__

class CPacket
{
public:
	//Packet Enum.
	enum en_PACKET
	{
		eBUFFER_DEFAULT = 2000		// 패킷의 기본 버퍼 사이즈.
	};

	// 생성자, 파괴자.
	CPacket();
	CPacket(int iBufferSize);
	virtual	~CPacket();

	// 패킷  파괴.
	void Release(void);

	// 패킷 청소. 
	void Clear(void);

	// 버퍼 사이즈 얻기. <return :버퍼 사이즈>
	int	GetBufferSize(void) { return m_iBufferSize; }

	// 현재 사용중인 사이즈 얻기. <return : 사용중인 데이터 사이즈>
	int	GetDataSize(void) { return m_iDataSize; }



	// 버퍼 포인터 얻기. 
	// <return : 버퍼 포인터>
	char *GetBufferPtr(void) { return m_chpBuffer; }
	char *GetReadPtr(void) { return m_chpReadPos; }
	char *GetWritePtr(void) { return m_chpWritePos; }

	// 버퍼 Pos 이동. (음수이동은 안됨)
	// GetBufferPtr 함수를 이용하여 외부에서 강제로 버퍼 내용을 수정할 경우 사용. 
	// <return	: 이동된 사이즈
	// (Parameters : 이동 사이즈)
	int		MoveWritePos(int iSize);
	int		MoveReadPos(int iSize);




	// 연산자 오버로딩
	CPacket	&operator = (CPacket &clSrcPacket);

	// 넣기.	각 변수 타입마다 모두 만듦.
	CPacket	&operator << (BYTE byValue);
	CPacket	&operator << (char chValue);

	CPacket	&operator << (short shValue);
	CPacket	&operator << (WORD wValue);

	CPacket	&operator << (int iValue);
	CPacket	&operator << (DWORD dwValue);
	CPacket	&operator << (float fValue);

	CPacket	&operator << (__int64 iValue);
	CPacket	&operator << (double dValue);

	CPacket& operator << (UINT64 iValue);
	CPacket& operator << (UINT iValue);



	// 빼기.	각 변수 타입마다 모두 만듦.
	CPacket	&operator>>(BYTE &byValue);
	CPacket	&operator>>(char &chValue);

	CPacket	&operator>>(short &shValue);
	CPacket	&operator>>(WORD &wValue);

	CPacket	&operator>>(int &iValue);
	CPacket	&operator>>(DWORD &dwValue);
	CPacket	&operator>>(float &fValue);

	CPacket	&operator>>(__int64 &iValue);
	CPacket	&operator>>(double &dValue);

	CPacket& operator >> (UINT64 & iValue);
	CPacket& operator >> (UINT & iValue);



	// 데이터 얻기.
	// <return	: (int)복사한 사이즈>
	// (parameters : (char *)Dest 포인터, (int)Size >
	int		GetData(char *chpDest, int iSize);

	// 데이터 삽입.
	// <return	: (int)복사한 사이즈. >
	// (Parameters : (char *)Src 포인터, (int)SrcSize )
	int		PutData(char *chpSrc, int iSrcSize);


	int		PeekData(char *chpDest, int iSize);


protected:

	//버퍼 사이즈
	int				m_iBufferSize;

	// 현재 버퍼에 사용중인 사이즈.
	int				m_iDataSize;

	//버퍼 포인터
	char*			m_chpBuffer;

	//버퍼의 읽기 위치, 쓰기 위치
	char*			m_chpReadPos;		//head
	char*			m_chpWritePos;		//rear

};



#endif