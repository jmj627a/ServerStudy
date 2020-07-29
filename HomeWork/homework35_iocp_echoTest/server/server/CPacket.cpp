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
	memset(m_chpBuffer, 0, m_iBufferSize);
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
	try
	{
		delete[] m_chpBuffer;
	}
	catch (...)
	{

	}
}

void CPacket::Clear(void)
{
	memset(m_chpBuffer, 0, m_iBufferSize);
	m_chpReadPos = m_chpBuffer;
	m_chpWritePos = m_chpBuffer;
	m_iDataSize = 0;
}

int CPacket::MoveWritePos(int iSize)
{
	int Usage = m_iBufferSize - (m_chpWritePos - m_chpReadPos);
	iSize = min(iSize, Usage);
	m_chpWritePos += iSize;
	m_iDataSize += iSize;

	return iSize;
}

int CPacket::MoveReadPos(int iSize)
{
	int Usage = m_chpWritePos - m_chpReadPos;
	iSize = min(iSize, Usage);
	m_chpReadPos += iSize;
	m_iDataSize -= iSize;

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

CPacket& CPacket::operator<<(UINT64 iValue)
{
	PutData((char*)(&iValue), sizeof(UINT64));
	return *this;
}

CPacket& CPacket::operator<<(UINT iValue)
{
	PutData((char*)(&iValue), sizeof(UINT));
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

CPacket& CPacket::operator>>(UINT64 & iValue)
{
	GetData((char*)(&iValue), sizeof(UINT64));
	return *this;
}

CPacket& CPacket::operator>>(UINT & iValue)
{
	GetData((char*)(&iValue), sizeof(UINT));
	return *this;
}

int CPacket::PickData(char* chpDest, int iSize)
{
	//iSize만큼 데이터가 없다면
	if (iSize > m_iDataSize)
		return 0;

	memcpy(chpDest, m_chpReadPos, iSize);

	return iSize;
}

int CPacket::GetData(char * chpDest, int iSize)
{
	//iSize만큼 데이터가 없다면
	if (iSize > m_iDataSize)
		return 0;
		
	memcpy(chpDest, m_chpReadPos, iSize);

	m_chpReadPos += iSize;		//읽은만큼 전진 
	m_iDataSize -= iSize;		//읽은만큼 데이터 수 줄어듦
		
	return iSize;
}

int CPacket::PutData(char * chpSrc, int iSrcSize)
{
	if (m_chpWritePos - m_chpBuffer >= m_iBufferSize)
		return  0;

	//넣을 자리가 없다면.
	if (iSrcSize > m_iBufferSize - m_iDataSize)
		return 0;

	memcpy(m_chpWritePos, chpSrc, iSrcSize);
	m_chpWritePos += iSrcSize;

	m_iDataSize += iSrcSize;

	return iSrcSize;
}