//////////////////////////////////////////////////////////////////////
// LogicPalette.cpp

#include <windows.h>
#include "KWnd.h"

#define PAL_NUM_ENTRIES  128
#define NUM_RECT  16

typedef struct {
	LOGPALETTE lp;
	PALETTEENTRY ape[PAL_NUM_ENTRIES - 1];
} LogPal;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//====================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	KWnd mainWnd("Colors with logic palette", hInstance, nCmdShow, WndProc, NULL, 0, 0, 808, 200);	

	while (GetMessage(&msg, NULL, 0, 0))  {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

//====================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hDC;
	PAINTSTRUCT ps;
	RECT r0;
	RECT rect[NUM_RECT];
	int dW, dH, i;

	static LogPal pal;
	LOGPALETTE* pLP = (LOGPALETTE*) &pal;

	static COLORREF paletteColor[NUM_RECT];
	static HBRUSH brush[NUM_RECT];
	static HPALETTE hPal;
	HPALETTE hOldPal;

	BYTE green;
	BYTE stepPal, stepBrush;

	switch (uMsg)
	{
	case WM_CREATE:

		// Создаем логическую палитру
		pLP->palVersion = 0x300;  // номер версии Windows
		pLP->palNumEntries = PAL_NUM_ENTRIES; // число входов палитры
		
		stepPal = 256 / PAL_NUM_ENTRIES;
		green = 255;
		for (i = 0; i < PAL_NUM_ENTRIES; i++) {
			pLP->palPalEntry[i].peRed = 0;
			pLP->palPalEntry[i].peGreen = green;
			pLP->palPalEntry[i].peBlue = 0;
			pLP->palPalEntry[i].peFlags = 0;
			green -= stepPal;
		}
		hPal = CreatePalette (pLP);

		// Готовим кисти для рисования
		stepBrush = 256 / NUM_RECT;
		for (i = 0; i < NUM_RECT; ++i)
			brush[i] = CreateSolidBrush(PALETTERGB(0, stepBrush * i, 0));
		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		hOldPal = SelectPalette(hDC, hPal, FALSE);

		GetClientRect(hWnd, &r0);
		dW = r0.right / NUM_RECT;
		dH = r0.bottom;
		for (i = 0; i < NUM_RECT; ++i) {
			SetRect(&rect[i], i*dW, 0, (i+1)*dW, dH);
			FillRect(hDC, &rect[i], brush[i]);
		}
	
		SelectPalette(hDC, hOldPal, TRUE);
		EndPaint(hWnd, &ps);
		break;

	case WM_QUERYNEWPALETTE:
		hDC = GetDC(hWnd);
		SelectPalette(hDC, hPal, FALSE);
		if (RealizePalette(hDC))
			InvalidateRect(hWnd, NULL, TRUE);
		ReleaseDC(hWnd, hDC);
		break;

	case WM_PALETTECHANGED:
		hDC = GetDC(hWnd);
		SelectPalette(hDC, hPal, FALSE);
		if (RealizePalette(hDC))
			InvalidateRect(hWnd, NULL, TRUE);
		ReleaseDC(hWnd, hDC);
		break;

    case WM_DESTROY:
		DeleteObject(hPal);
		for (i = 0; i < NUM_RECT; ++i)
			DeleteObject(brush[i]);
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

