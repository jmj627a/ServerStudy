#include "CSpriteDib.h"

CSpriteDib::CSpriteDib(int iMaxSprite, DWORD dwColorKey)
{
	m_iMaxSprite = iMaxSprite;
	m_dwColorKey = dwColorKey;

	m_stpSprite = NULL;

	//�ִ� �о�� ���� ��ŭ �̸� �Ҵ� �޴´�
	m_stpSprite = new st_SPRITE[m_iMaxSprite];
	memset(m_stpSprite, 0, sizeof(st_SPRITE) * m_iMaxSprite);
}

CSpriteDib::~CSpriteDib()
{
	int iCount;

	//��ü�� ���鼭 ��� ������
	for (iCount = 0; iCount > m_iMaxSprite; iCount++)
	{
		ReleaseSprite(iCount);
	}
}

BOOL CSpriteDib::LoadDibSprite(int iSpriteIndex, const wchar_t* szFileName, int iCenterPointX, int iCenterPointY)
{
	HANDLE hFile;
	DWORD dwRead;
	int iPitch;
	int iImageSize;

	BITMAPFILEHEADER stFileHeader;
	BITMAPINFOHEADER stInfoHeader;

	//�ϴ� ������ ����
	hFile = CreateFile(szFileName, GENERIC_READ, NULL, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);

	if(INVALID_HANDLE_VALUE == hFile)
		return FALSE;

	//�ش� ��������Ʈ�� �̹� �����Ͱ� �ִٸ� �̸� ������
	ReleaseSprite(iSpriteIndex);

	//��Ʈ�� ����� ���� BMP ���� Ȯ��
	ReadFile(hFile, &stFileHeader, sizeof(BITMAPFILEHEADER), &dwRead, NULL);

	if (0x4d42 == stFileHeader.bfType)
	{
		//���� ����� �о ���� & 32��Ʈ Ȯ��
		ReadFile(hFile, &stInfoHeader, sizeof(BITMAPINFOHEADER), &dwRead, NULL);
		if (32 == stInfoHeader.biBitCount)
		{
			//���� ������ ��ġ���� ���Ѵ�
			iPitch = ((stInfoHeader.biWidth * 4) + 3) & ~3;

			//��������Ʈ ����ü�� ũ�� ����
			m_stpSprite[iSpriteIndex].iWidth = stInfoHeader.biWidth;
			m_stpSprite[iSpriteIndex].iHeight = stInfoHeader.biHeight;
			m_stpSprite[iSpriteIndex].iPitch = iPitch;

			//�̹����� ���� ��ü ũ�⸦ ���ϰ� �޸� �Ҵ�
			iImageSize = iPitch * stInfoHeader.biHeight;
			m_stpSprite[iSpriteIndex].bypImage = new BYTE[iImageSize];

			//�̹��� �κ��� ��������Ʈ ���۷� �о�´�
			//DIB�� �������� �����Ƿ� �̸� �ٽ� ������
			//�ӽù��ۿ� ���� �ڿ� �������鼭 �����Ѵ�
			BYTE* bypTempBuffer = new BYTE[iImageSize];
			BYTE* bypSpriteTemp = m_stpSprite[iSpriteIndex].bypImage;
			BYTE* bypTurnTemp;
			int iCount;

			ReadFile(hFile, bypTempBuffer, iImageSize, &dwRead, NULL);

			//�������� ������
			bypTurnTemp = bypTempBuffer + iPitch * (stInfoHeader.biHeight - 1);

			for (iCount = 0; iCount < stInfoHeader.biHeight; iCount++)
			{
				memcpy(bypSpriteTemp, bypTurnTemp, iPitch);
				bypSpriteTemp += iPitch;
				bypTurnTemp -= iPitch;
			}

			delete[] bypTempBuffer;

			m_stpSprite[iSpriteIndex].iCenterPointX = iCenterPointX;
			m_stpSprite[iSpriteIndex].iCenterPointY = iCenterPointY;

			CloseHandle(hFile);
			return TRUE;
		}
	}

	CloseHandle(hFile);
	return false;
}

void CSpriteDib::ReleaseSprite(int iSpriteIndex)
{
	//�ִ� �Ҵ�� ��������Ʈ�� �Ѿ�� �ȵ�����
	if (m_iMaxSprite <= iSpriteIndex)
		return;

	if (NULL != m_stpSprite[iSpriteIndex].bypImage)
	{
		//������ �ʱ�ȭ
		delete[] m_stpSprite[iSpriteIndex].bypImage;
		memset(&m_stpSprite[iSpriteIndex], 0, sizeof(st_SPRITE));
	}
}

extern HWND hWnd;
void CSpriteDib::DrawSprite(int iSpriteIndex, int iDrawX, int iDrawY, BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch, int iDrawLen)
{
	//�ִ� ��������Ʈ ������ �ʰ��ϰų� �ε���� �ʴ� ��������Ʈ��� ����
	if (iSpriteIndex >= m_iMaxSprite)
		return;

	if (NULL == m_stpSprite[iSpriteIndex].bypImage)
		return;

	st_SPRITE* stpSprite = &m_stpSprite[iSpriteIndex];

	//��������Ʈ ����� ���� ������ ����
	int iSpriteWidth = stpSprite->iWidth;
	int iSpriteHeight = stpSprite->iHeight;
	int iCountX, iCountY;

	iSpriteWidth = iSpriteWidth * iDrawLen / 100;

	DWORD* dwpDest = (DWORD*)bypDest;
	DWORD* dwpSprite = (DWORD*)(stpSprite->bypImage);

	//��� ���� ó�� 
	iDrawX = iDrawX - stpSprite->iCenterPointX;
	iDrawY = iDrawY - stpSprite->iCenterPointY;

	//��ܿ� ���� ��������Ʈ ��� ��ġ ��� (��� Ŭ����)
	if (0 > iDrawY)
	{
		iSpriteHeight = iSpriteHeight - (-iDrawY);
		dwpSprite = (DWORD*)(stpSprite->bypImage + stpSprite->iPitch * (-iDrawY));

		//������ ©���� ����̹Ƿ� ��������Ʈ ���� ��ġ�� �Ʒ��� �����ش� 
		iDrawY = 0;
	}

	//�ϴܿ� ������ ��� (�ϴ� Ŭ����)
	if (iDestHeight <= iDrawY + stpSprite->iHeight)
		iSpriteHeight -= ((iDrawY + stpSprite->iHeight) - iDestHeight);


	//���� ��� ��ġ ��� (���� Ŭ����)
	if (0 > iDrawX)
	{
		iSpriteWidth = iSpriteWidth - (-iDrawX);
		dwpSprite = dwpSprite + (-iDrawX);

		//������ ���� �߸��Ƿ� ��� ���� ��ġ�� ���������� �δ�
		iDrawX = 0;
	}

	//������ ��� ��ġ ��� (���� Ŭ����)
	if (iDestWidth <= iDrawX + stpSprite->iWidth)
		iSpriteWidth -= ((iDrawX + stpSprite->iWidth) - iDestWidth);

	//���� �׸��� ���ٸ� �����Ѵ�
	if (iSpriteWidth <= 0 || iSpriteHeight <= 0)
		return;

	//ȭ�鿡 ���� ��ġ�� �̵��Ѵ� 
	dwpDest = (DWORD*)(((BYTE*)(dwpDest + iDrawX) + (iDrawY * iDestPitch)));

	BYTE* bypDestOrigin = (BYTE*)dwpDest;
	BYTE* bypSpriteOrigin = (BYTE*)dwpSprite;

	//��ü ũ�⸦ ���鼭 ����� ó���� �Ѵ�
	for (iCountY = 0; iSpriteHeight > iCountY; iCountY++)
	{
		for (iCountX = 0; iSpriteWidth > iCountX; iCountX++)
		{
			if (m_dwColorKey != (*dwpSprite & 0x00ffffff))
				*dwpDest = *dwpSprite;
			
			dwpDest++;
			dwpSprite++;
		}
	
		//�����ٷ� �̵�, ȭ��� ��������Ʈ ���
		bypDestOrigin = bypDestOrigin + iDestPitch;
		bypSpriteOrigin = bypSpriteOrigin + stpSprite->iPitch;

		dwpDest = (DWORD*)bypDestOrigin;
		dwpSprite = (DWORD*)bypSpriteOrigin;
	
	}

}

void CSpriteDib::DrawMySprite(int iSpriteIndex, int iDrawX, int iDrawY, BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch, int iDrawLen)
{
	//�ִ� ��������Ʈ ������ �ʰ��ϰų� �ε���� �ʴ� ��������Ʈ��� ����
	if (iSpriteIndex >= m_iMaxSprite)
		return;

	if (NULL == m_stpSprite[iSpriteIndex].bypImage)
		return;

	st_SPRITE* stpSprite = &m_stpSprite[iSpriteIndex];

	//��������Ʈ ����� ���� ������ ����
	int iSpriteWidth = stpSprite->iWidth;
	int iSpriteHeight = stpSprite->iHeight;
	int iCountX, iCountY;

	iSpriteWidth = iSpriteWidth * iDrawLen / 100;

	DWORD* dwpDest = (DWORD*)bypDest;
	DWORD* dwpSprite = (DWORD*)(stpSprite->bypImage);

	//��� ���� ó�� 
	iDrawX = iDrawX - stpSprite->iCenterPointX;
	iDrawY = iDrawY - stpSprite->iCenterPointY;

	//��ܿ� ���� ��������Ʈ ��� ��ġ ��� (��� Ŭ����)
	if (0 > iDrawY)
	{
		iSpriteHeight = iSpriteHeight - (-iDrawY);
		dwpSprite = (DWORD*)(stpSprite->bypImage + stpSprite->iPitch * (-iDrawY));

		//������ ©���� ����̹Ƿ� ��������Ʈ ���� ��ġ�� �Ʒ��� �����ش� 
		iDrawY = 0;
	}

	//�ϴܿ� ������ ��� (�ϴ� Ŭ����)
	if (iDestHeight <= iDrawY + stpSprite->iHeight)
		iSpriteHeight -= ((iDrawY + stpSprite->iHeight) - iDestHeight);


	//���� ��� ��ġ ��� (���� Ŭ����)
	if (0 > iDrawX)
	{
		iSpriteWidth = iSpriteWidth - (-iDrawX);
		dwpSprite = dwpSprite + (-iDrawX);

		//������ ���� �߸��Ƿ� ��� ���� ��ġ�� ���������� �δ�
		iDrawX = 0;
	}

	//������ ��� ��ġ ��� (���� Ŭ����)
	if (iDestWidth <= iDrawX + stpSprite->iWidth)
		iSpriteWidth -= ((iDrawX + stpSprite->iWidth) - iDestWidth);

	//���� �׸��� ���ٸ� �����Ѵ�
	if (iSpriteWidth <= 0 || iSpriteHeight <= 0)
		return;

	//ȭ�鿡 ���� ��ġ�� �̵��Ѵ� 
	dwpDest = (DWORD*)(((BYTE*)(dwpDest + iDrawX) + (iDrawY * iDestPitch)));

	BYTE* bypDestOrigin = (BYTE*)dwpDest;
	BYTE* bypSpriteOrigin = (BYTE*)dwpSprite;

	//��ü ũ�⸦ ���鼭 ����� ó���� �Ѵ�
	int	R1, G1, B1;
	int R2, G2, B2;
	for (iCountY = 0; iSpriteHeight > iCountY; iCountY++)
	{
		for (iCountX = 0; iSpriteWidth > iCountX; iCountX++)
		{
			if (m_dwColorKey != (*dwpSprite & 0x00ffffff))
			{
				R1 = (*dwpDest & 0x00ff0000) >> 16;
				G1 = (*dwpDest & 0x0000ff00) >> 8;
				B1 = (*dwpDest & 0x000000ff) >> 0;

				R2 = (*dwpSprite & 0x00ff0000) >> 16;
				G2 = (*dwpSprite & 0x0000ff00) >> 8;
				B2 = (*dwpSprite & 0x000000ff) >> 0;

				//*dwpDest = (R1 / 2 + R2 / 2 << 16) | (G1 / 2 + G2 / 2 << 16) | (B1 / 2 + B2 / 2);
				*dwpDest = (R2<<16) | (G2/2<<8) | (B2);

				//*dwpDest = *dwpSprite;
			}

			dwpDest++;
			dwpSprite++;
		}

		//�����ٷ� �̵�, ȭ��� ��������Ʈ ���
		bypDestOrigin = bypDestOrigin + iDestPitch;
		bypSpriteOrigin = bypSpriteOrigin + stpSprite->iPitch;

		dwpDest = (DWORD*)bypDestOrigin;
		dwpSprite = (DWORD*)bypSpriteOrigin;
	}
}

void CSpriteDib::DrawSprite50(int iSpriteIndex, int iDrawX, int iDrawY, BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch, int iDrawLen)
{
	//�ִ� ��������Ʈ ������ �ʰ��ϰų� �ε���� �ʴ� ��������Ʈ��� ����
	if (iSpriteIndex >= m_iMaxSprite)
		return;

	if (NULL == m_stpSprite[iSpriteIndex].bypImage)
		return;

	st_SPRITE* stpSprite = &m_stpSprite[iSpriteIndex];

	//��������Ʈ ����� ���� ������ ����
	int iSpriteWidth = stpSprite->iWidth;
	int iSpriteHeight = stpSprite->iHeight;
	int iCountX, iCountY;

	iSpriteWidth = iSpriteWidth * iDrawLen / 100;

	DWORD* dwpDest = (DWORD*)bypDest;
	DWORD* dwpSprite = (DWORD*)(stpSprite->bypImage);

	//��� ���� ó�� 
	iDrawX = iDrawX - stpSprite->iCenterPointX;
	iDrawY = iDrawY - stpSprite->iCenterPointY;

	//��ܿ� ���� ��������Ʈ ��� ��ġ ��� (��� Ŭ����)
	if (0 > iDrawY)
	{
		iSpriteHeight = iSpriteHeight - (-iDrawY);
		dwpSprite = (DWORD*)(stpSprite->bypImage + stpSprite->iPitch * (-iDrawY));

		//������ ©���� ����̹Ƿ� ��������Ʈ ���� ��ġ�� �Ʒ��� �����ش� 
		iDrawY = 0;
	}

	//�ϴܿ� ������ ��� (�ϴ� Ŭ����)
	if (iDestHeight <= iDrawY + stpSprite->iHeight)
		iSpriteHeight -= ((iDrawY + stpSprite->iHeight) - iDestHeight);


	//���� ��� ��ġ ��� (���� Ŭ����)
	if (0 > iDrawX)
	{
		iSpriteWidth = iSpriteWidth - (-iDrawX);
		dwpSprite = dwpSprite + (-iDrawX);

		//������ ���� �߸��Ƿ� ��� ���� ��ġ�� ���������� �δ�
		iDrawX = 0;
	}

	//������ ��� ��ġ ��� (���� Ŭ����)
	if (iDestWidth <= iDrawX + stpSprite->iWidth)
		iSpriteWidth -= ((iDrawX + stpSprite->iWidth) - iDestWidth);

	//���� �׸��� ���ٸ� �����Ѵ�
	if (iSpriteWidth <= 0 || iSpriteHeight <= 0)
		return;

	//ȭ�鿡 ���� ��ġ�� �̵��Ѵ� 
	dwpDest = (DWORD*)(((BYTE*)(dwpDest + iDrawX) + (iDrawY * iDestPitch)));

	BYTE* bypDestOrigin = (BYTE*)dwpDest;
	BYTE* bypSpriteOrigin = (BYTE*)dwpSprite;

	//��ü ũ�⸦ ���鼭 ����� ó���� �Ѵ�
	int	R1, G1, B1;
	int R2, G2, B2;
	for (iCountY = 0; iSpriteHeight > iCountY; iCountY++)
	{
		for (iCountX = 0; iSpriteWidth > iCountX; iCountX++)
		{
			if (m_dwColorKey != (*dwpSprite & 0x00ffffff))
			{
				R1 = (*dwpDest & 0x00ff0000) >> 16;
				G1 = (*dwpDest & 0x0000ff00) >> 8;
				B1 = (*dwpDest & 0x000000ff) >> 0;

				R2 = (*dwpSprite & 0x00ff0000) >> 16;
				G2 = (*dwpSprite & 0x0000ff00) >> 8;
				B2 = (*dwpSprite & 0x000000ff) >> 0;

				*dwpDest = (R1 / 2 + R2 / 2 << 16) | (G1 / 2 + G2 / 2 << 8) | (B1 / 2 + B2 / 2);
				//*dwpDest = *dwpSprite;
			}

	
			dwpDest++;
			dwpSprite++;
		}

		//�����ٷ� �̵�, ȭ��� ��������Ʈ ���
		bypDestOrigin = bypDestOrigin + iDestPitch;
		bypSpriteOrigin = bypSpriteOrigin + stpSprite->iPitch;

		dwpDest = (DWORD*)bypDestOrigin;
		dwpSprite = (DWORD*)bypSpriteOrigin;
	}
}

void CSpriteDib::DrawImage(int iSpriteIndex, int iDrawX, int iDrawY, BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch, int iDrawLen)
{
	DrawSprite(iSpriteIndex, iDrawX, iDrawY, bypDest, iDestWidth, iDestHeight, iDestPitch, iDrawLen);
}
