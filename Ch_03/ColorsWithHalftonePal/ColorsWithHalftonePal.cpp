//////////////////////////////////////////////////////////////////////
// ColorsWithHalftonePal.cpp
#include <windows.h>
#include "KWnd.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//====================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	KWnd mainWnd("Colors with halftone palette", hInstance, nCmdShow, WndProc, NULL, 0, 0, 800, 200);	

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
	RECT rect[8];
	int dW, dH, i;

	COLORREF paletteColor[8] = { PALETTERGB(128,0,0), PALETTERGB(192,0,0), PALETTERGB(255,0,0),
		PALETTERGB(255,192,0), PALETTERGB(176,250,133), PALETTERGB(245,197,137),
		PALETTERGB(255,128,128), PALETTERGB(255,128,255)};
	
	static HBRUSH brush[8];
	static HPALETTE hPal;
	HPALETTE hOldPal;

	switch (uMsg)
	{
	case WM_CREATE:

		for (i = 0; i < 8; ++i)
			brush[i] = CreateSolidBrush(paletteColor[i]);

		hDC = GetDC(hWnd);
		hPal = CreateHalftonePalette(hDC);
		ReleaseDC(hWnd, hDC);
		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		hOldPal = SelectPalette(hDC, hPal, FALSE);

		GetClientRect(hWnd, &r0);
		dW = r0.right / 8;
		dH = r0.bottom;
		for (i = 0; i < 8; ++i) {
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
		for (i = 0; i < 8; ++i)
			DeleteObject(brush[i]);
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}
