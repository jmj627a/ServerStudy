#include "stdafx.h"
#include "CPacket.h"



CPacket::CPacket()
{
	m_chpBuffer = new char[eBUFFER_DEFAULT];
	m_iBufferSize = eBUFFER_DEFAULT;

	m_chpReadPos  = m_chpBuffer;
	m_chpWritePos = m_chpBuffer;

	m_iDataSize = 0;
}

CPacket::CPacket(int iBufferSize)
{
	m_chpBuffer = new char[iBufferSize];
	m_iBufferSize = iBufferSize;

	m_chpReadPos = m_chpBuffer;
	m_chpWritePos = m_chpBuffer;

	m_iDataSize = 0;
}


CPacket::~CPacket()
{
	Release();
}

void CPacket::Release(void)
{
	delete[] m_chpBuffer;
}

void CPacket::Clear(void)
{
	memset(m_chpBuffer, 0, m_iBufferSize);
	m_chpReadPos = m_chpBuffer;
	m_chpWritePos = m_chpBuffer;
}

int CPacket::MoveWritePos(int iSize)
{
	m_chpWritePos = m_chpWritePos + iSize;
	m_iDataSize += iSize;

	return iSize;
}

int CPacket::MoveReadPos(int iSize)
{
	m_chpReadPos = m_chpReadPos + iSize;
	m_iDataSize += iSize;

	return iSize;
}



CPacket & CPacket::operator=(CPacket & clSrcPacket)
{
	m_chpReadPos = m_chpBuffer;
	m_chpWritePos = m_chpBuffer;

	//원본 패킷 클래스에서 복사
	PutData(clSrcPacket.m_chpReadPos, clSrcPacket.m_iDataSize);

	return *this;
}


//넣기
CPacket & CPacket::operator<<(BYTE byValue)
{
	PutData((char*)(&byValue), sizeof(BYTE));
	return *this;
}

CPacket & CPacket::operator<<(short shValue)
{
	PutData((char*)(&shValue), sizeof(short));
	return *this;
}

CPacket & CPacket::operator<<(WORD wValue)
{
	PutData((char*)(&wValue), sizeof(WORD));
	return *this;
}

CPacket & CPacket::operator<<(int iValue)
{
	PutData((char*)(&iValue), sizeof(int));
	return *this;
}

CPacket & CPacket::operator<<(DWORD dwValue)
{
	PutData((char*)(&dwValue), sizeof(DWORD));
	return *this;
}

CPacket & CPacket::operator<<(float fValue)
{
	PutData((char*)(&fValue), sizeof(float));
	return *this;
}

CPacket & CPacket::operator<<(__int64 iValue)
{
	PutData((char*)(&iValue), sizeof(__int64));
	return *this;
}

CPacket & CPacket::operator<<(double dValue)
{
	PutData((char*)(&dValue), sizeof(double));
	return *this;
}

CPacket & CPacket::operator<<(char chValue)
{
	PutData((char*)(&chValue), sizeof(char));
	return *this;
}



//빼기
CPacket & CPacket::operator>>(BYTE & byValue)
{
	GetData((char*)(&byValue), sizeof(BYTE));
	return *this;
}

CPacket & CPacket::operator>>(char & chValue)
{
	GetData((char*)(&chValue), sizeof(char));
	return *this;
}

CPacket & CPacket::operator>>(short & shValue)
{
	GetData((char*)(&shValue), sizeof(short));
	return *this;
}

CPacket & CPacket::operator>>(WORD & wValue)
{
	GetData((char*)(&wValue), sizeof(WORD));
	return *this;
}

CPacket & CPacket::operator>>(int & iValue)
{
	GetData((char*)(&iValue), sizeof(int));
	return *this;
}

CPacket & CPacket::operator>>(DWORD & dwValue)
{
	GetData((char*)(&dwValue), sizeof(DWORD));
	return *this;
}

CPacket & CPacket::operator>>(float & fValue)
{
	GetData((char*)(&fValue), sizeof(float));
	return *this;
}

CPacket & CPacket::operator>>(__int64 & iValue)
{
	GetData((char*)(&iValue), sizeof(__int64));
	return *this;
}

CPacket & CPacket::operator>>(double & dValue)
{
	GetData((char*)(&dValue), sizeof(double));
	return *this;
}

int CPacket::GetData(char * chpDest, int iSize)
{
	int count;

	for (count = 0; count < iSize; count++)
	{
		chpDest[count] = *m_chpReadPos;
		m_chpReadPos++;
		m_iDataSize--;
	}

	return count;
}

int CPacket::PutData(char * chpSrc, int iSrcSize)
{
	int count;

	for (count = 0; count < iSrcSize; count++)
	{
		*m_chpWritePos = chpSrc[count];
		m_chpWritePos++;
		m_iDataSize++;
	}

	return count;
}