////////////////////////////////////////////////////////////
// BmpFileViewer2.cpp
#include <windows.h>
#include "KWnd.h"
#include "../BmpFileViewer/KDib.h"

#define FILE_NAME "YoungHacker.bmp"

KDib bmp;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//====================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	KWnd mainWnd("BmpFileViewer", hInstance, nCmdShow, WndProc);	

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
	RECT rect;
	int dX, dY;
	int ws, hs, wd, hd;
	static BOOL isFileLoaded;
	int xd0, yd0, xd1, yd1, xd2, yd2, xd3, yd3;

	switch (uMsg)
	{

	case WM_CREATE:
		hDC = GetDC(hWnd);

		isFileLoaded = bmp.LoadFromFile(FILE_NAME);
		if (!isFileLoaded) {
			MessageBox(hWnd, "Файл " FILE_NAME " не загружен.", "Error", MB_OK);
			MessageBox(hWnd, bmp.GetError(), "Error", MB_OK);
			break;
		}
		// Подогнать размеры окна программы под размер растра bmp
		GetClientRect(hWnd, &rect);
		dX = bmp.GetWidth() - rect.right;
		dY = bmp.GetHeight() - rect.bottom;
		GetWindowRect(hWnd, &rect);
		InflateRect(&rect, dX/2, dY/2);

		MoveWindow(hWnd, 0, 0,
			rect.right-rect.left, rect.bottom-rect.top, TRUE);


		ReleaseDC(hWnd, hDC);
		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);

		ws = bmp.GetWidth();
		hs = bmp.GetHeight();
		wd = ws/2 - 4;
		hd = hs/2 - 4;
		xd0 = 2;         yd0 = 2;
		xd1 = ws/2 + 2;  yd1 = 2;
		xd2 = 2;         yd2 = hs/2 + 2;
		xd3 = ws/2 + 2;  yd3 = hs/2 + 2;

		SetStretchBltMode(hDC, STRETCH_HALFTONE);
		bmp.Draw(hDC, xd0, yd0, wd, hd, ws,  0, -ws,  hs, SRCCOPY);
		bmp.Draw(hDC, xd1, yd1, wd, hd,  0,  0,  ws,  hs, SRCCOPY);
		bmp.Draw(hDC, xd2, yd2, wd, hd, ws, hs, -ws, -hs, SRCCOPY);
		bmp.Draw(hDC, xd3, yd3, wd, hd,  0, hs,  ws, -hs, SRCCOPY);

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
//////////////////////////////////////////////////////////////////////
