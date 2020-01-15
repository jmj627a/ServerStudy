#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <Windows.h>

void main()
{
	FILE* pImageFile1 = fopen("sample.bmp", "rb");
	FILE* pImageFile2 = fopen("sample2.bmp", "rb");
	FILE* pImageFile3 = fopen("blend.bmp", "wb");

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


	char* pImageBuffer1;
	char* pImageBuffer2;
	char* pImageBuffer3;

	int imageSize = (infoHeader1.biBitCount / 8) * infoHeader1.biWidth * infoHeader1.biHeight;
	pImageBuffer1 = (char*)malloc(imageSize);
	pImageBuffer2 = (char*)malloc(imageSize);
	pImageBuffer3 = (char*)malloc(imageSize);

	fread(pImageBuffer1, 1, imageSize, pImageFile1);
	fread(pImageBuffer2, 1, imageSize, pImageFile2);
	fclose(pImageFile1);
	fclose(pImageFile2);

	int R1, R2, G1, G2, B1, B2;

	for (int i = 0; i < infoHeader1.biHeight; ++i)
	{
		for (int j = 0; j < infoHeader1.biWidth; ++j)
		{

		}
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