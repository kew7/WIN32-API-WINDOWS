// KDibSection.cpp
#include "KDibSection.h"

BOOL KDibSection::Create(HDC hdc, int _width, int _height) {
	width = _width;
	height = _height;

	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = -height;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 24;
	bmi.bmiHeader.biCompression = BI_RGB;

	bytePerLine = ((width * bmi.bmiHeader.biBitCount + 31) / 32) * 4;
		
	hBmp = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
	return (hBmp != NULL);
}
