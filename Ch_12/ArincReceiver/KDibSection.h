// KDibSection.h
#include <windows.h>

class KDibSection {
public:
	KDibSection() : pBits(0) {} 
	BOOL Create(HDC hdc, int _width, int _height);

	inline void SetPixel(int x, int y, COLORREF color) {
		int ind = 3 * x; 
		((BYTE*)pBits)[y*bytePerLine + ind] = GetBValue(color);
		((BYTE*)pBits)[y*bytePerLine + ind + 1] = GetGValue(color);
		((BYTE*)pBits)[y*bytePerLine + ind + 2] = GetRValue(color);
	}

	inline void Draw(HDC hdc) {
		StretchDIBits(hdc, 0, 0, width, height,
			0, 0, width, height, pBits, &bmi,
			DIB_RGB_COLORS, SRCCOPY);
	}
	
private:
	HBITMAP hBmp;
	BITMAPINFO bmi;
	int width;
	int height;
	int bytePerLine;
	PVOID pBits;
};