#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <Windows.h>

void main()
{
	FILE* pImageFile1 = fopen("sample.bmp", "rb");
	FILE* pImageFile2 = fopen("sample2.bmp", "rb");
	FILE* pImageFile3 = fopen("blend.bmp", "wb");

	//2는 사실 사진 두장이 같은 사이즈라 필요는 없음 
	BITMAPFILEHEADER fileHeader1, fileHeader2;
	BITMAPINFOHEADER infoHeader1, infoHeader2;

	int ret;
	ret = fread(&fileHeader1, 1, sizeof(BITMAPFILEHEADER), pImageFile1);
	if (sizeof(BITMAPFILEHEADER) != ret)
		return;

	ret = fread(&fileHeader2, 1, sizeof(BITMAPFILEHEADER), pImageFile2);
	if (sizeof(BITMAPFILEHEADER) != ret)
		return;

	ret = fread(&infoHeader1, 1, sizeof(BITMAPINFOHEADER), pImageFile1);
	if (sizeof(BITMAPINFOHEADER) != ret)
		return;

	ret = fread(&infoHeader2, 1, sizeof(BITMAPINFOHEADER), pImageFile2);
	if (sizeof(BITMAPINFOHEADER) != ret)
		return;


	unsigned int* pImageBuffer1;
	unsigned int* pImageBuffer2;
	unsigned int* pImageBuffer3;

	int imageSize = (infoHeader1.biBitCount / 8) * infoHeader1.biWidth * infoHeader1.biHeight;
	pImageBuffer1 = (unsigned int*)malloc(imageSize);
	pImageBuffer2 = (unsigned int*)malloc(imageSize);
	pImageBuffer3 = (unsigned int*)malloc(imageSize);

	fread(pImageBuffer1, 1, imageSize, pImageFile1);
	fread(pImageBuffer2, 1, imageSize, pImageFile2);
	fclose(pImageFile1);
	fclose(pImageFile2);


	//원본을 훼손하지 않고 포인터 ++ 해서 대입하기 위해
	unsigned int* pImage1 = pImageBuffer1;
	unsigned int* pImage2 = pImageBuffer2;
	unsigned int* pImage3 = pImageBuffer3;
				
	//첫출을 저장하기 위해
	unsigned int* origin1 = pImageBuffer1;
	unsigned int* origin2 = pImageBuffer2;
	unsigned int* origin3 = pImageBuffer3;

	//A R G B 순서
	int	R1, R2, G1, G2, B1, B2;

	for (int i = 0; i < infoHeader1.biHeight; ++i)
	{
		for (int j = 0; j < infoHeader1.biWidth; ++j)
		{
			R1 = (*pImage1 & 0x00ff0000) >> 16;
			G1 = (*pImage1 & 0x0000ff00) >> 8;
			B1 = (*pImage1 & 0x000000ff) >> 0;

			R2 = (*pImage2 & 0x00ff0000) >> 16;
			G2 = (*pImage2 & 0x0000ff00) >> 8;
			B2 = (*pImage2 & 0x000000ff) >> 0;

			//Gray Scale
			//unsigned int temp = (R1 + G1 + B1) / 3;
			//*pImage3 = (temp << 16) | (temp << 8) | (temp );

			//반반 섞기
			//*pImage3 = (R1 / 2 + R2 / 2 << 16) | (G1 / 2 + G2 / 2 << 8) | (B1 / 2 + B2 / 2);
			
			//*pImage3 = (R1 << 16) | (G1 << 8) | (B1);
			
			//*pImage3 = *pImage1;
			//*pImage3 = *pImage2;
			
			pImage1++;
			pImage2++;
			pImage3++;
		}

		//새로운 줄 첫부분에서 복사를 시작하기 위해
		origin1 += infoHeader1.biWidth;
		origin2 += infoHeader1.biWidth;
		origin3 += infoHeader1.biWidth;

		//첫 부분으로 옮겨줌
		pImage1 = origin1;
		pImage2 = origin2;
		pImage3 = origin3;
	}
	
	//memcpy(pImageBuffer3, pImageBuffer1, imageSize);

	fwrite(&fileHeader1, 1, sizeof(BITMAPFILEHEADER), pImageFile3);
	fwrite(&infoHeader1, 1, sizeof(BITMAPINFOHEADER), pImageFile3);
	fwrite(pImageBuffer3, 1, imageSize, pImageFile3);

	fclose(pImageFile3);

	free(pImageBuffer1);
	free(pImageBuffer2);
	free(pImageBuffer3);

}