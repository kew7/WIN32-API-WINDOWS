//////////////////////////////////////////////////////////////////////
// CreateCharBmp.cpp
#include <windows.h>
#include "KWnd.h"
#include "KDib.h"

#define ANSI_CODE  65
#define FILE_NAME "symbol.bmp"

KDib bmp;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//====================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	KWnd mainWnd("CreateCharBmp", hInstance, nCmdShow, WndProc);	

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
	TEXTMETRIC tm;
	SIZE sz;
	static int width, height;
	COLORREF color;
	int x, y;
	BOOL isFileCreated;
	static char line[2];

	switch (uMsg)
	{

	case WM_CREATE:
		hDC = GetDC(hWnd);
		GetTextMetrics(hDC, &tm);
		line[0] = ANSI_CODE;
		line[1] = 0;
		GetTextExtentPoint32(hDC, line, 1, &sz);
		width = sz.cx;
		height = tm.tmHeight;

		isFileCreated =	bmp.CreateDib24(width, height, FILE_NAME);
		if (!isFileCreated)
			MessageBox(hWnd, "Файл " FILE_NAME " не создан.", "Error", MB_OK);

		ReleaseDC(hWnd, hDC);
		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		SetBkColor(hDC, RGB(0,0,0));
		SetTextColor(hDC, RGB(255,255,255));

		TextOut(hDC, 0, 0, line, 1);

		// Сканирование изображения символа (от последней строки к первой)
		// Запись в массив пикселов DIB
		for (y = 0; y < height; ++y)	{
			for (x = 0; x < width; ++x) {
				color = GetPixel(hDC, x, height-1-y);
				bmp.SetPixel(x, y, color);
			}
		}
		
		bmp.StoreDib24();

		EndPaint(hWnd, &ps);
		break;

    case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}
