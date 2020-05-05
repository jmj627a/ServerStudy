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
		eBUFFER_DEFAULT = 1400		// ��Ŷ�� �⺻ ���� ������.
	};

	// ������, �ı���.
	CPacket();
	CPacket(int iBufferSize);
	virtual	~CPacket();

	// ��Ŷ  �ı�.
	void Release(void);

	// ��Ŷ û��. 
	void Clear(void);

	// ���� ������ ���. <return :���� ������>
	int	GetBufferSize(void) { return m_iBufferSize; }

	// ���� ������� ������ ���. <return : ������� ������ ������>
	int	GetDataSize(void) { return m_iDataSize; }



	// ���� ������ ���. 
	// <return : ���� ������>
	char *GetBufferPtr(void) { return m_chpBuffer; }

	// ���� Pos �̵�. (�����̵��� �ȵ�)
	// GetBufferPtr �Լ��� �̿��Ͽ� �ܺο��� ������ ���� ������ ������ ��� ���. 
	// <return	: �̵��� ������
	// (Parameters : �̵� ������)
	int		MoveWritePos(int iSize);
	int		MoveReadPos(int iSize);




	// ������ �����ε�
	CPacket	&operator = (CPacket &clSrcPacket);

	// �ֱ�.	�� ���� Ÿ�Ը��� ��� ����.
	CPacket	&operator << (BYTE byValue);
	CPacket	&operator << (char chValue);

	CPacket	&operator << (short shValue);
	CPacket	&operator << (WORD wValue);

	CPacket	&operator << (int iValue);
	CPacket	&operator << (DWORD dwValue);
	CPacket	&operator << (float fValue);

	CPacket	&operator << (__int64 iValue);
	CPacket	&operator << (double dValue);


	// ����.	�� ���� Ÿ�Ը��� ��� ����.
	CPacket	&operator >> (BYTE &byValue);
	CPacket	&operator >> (char &chValue);

	CPacket	&operator >> (short &shValue);
	CPacket	&operator >> (WORD &wValue);

	CPacket	&operator >> (int &iValue);
	CPacket	&operator >> (DWORD &dwValue);
	CPacket	&operator >> (float &fValue);

	CPacket	&operator >> (__int64 &iValue);
	CPacket	&operator >> (double &dValue);




	// ������ ���.
	// <return	: (int)������ ������>
	// (parameters : (char *)Dest ������, (int)Size >
	int		GetData(char *chpDest, int iSize);

	// ������ ����.
	// <return	: (int)������ ������. >
	// (Parameters : (char *)Src ������, (int)SrcSize )
	int		PutData(char *chpSrc, int iSrcSize);




protected:

	//���� ������
	int				m_iBufferSize;

	// ���� ���ۿ� ������� ������.
	int				m_iDataSize;

	//���� ������
	char*			m_chpBuffer;

	//������ �б� ��ġ, ���� ��ġ
	char*			m_chpReadPos;		//head
	char*			m_chpWritePos;		//rear

};



#endif