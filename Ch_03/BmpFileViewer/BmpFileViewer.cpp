////////////////////////////////////////////////////////////
// BmpFileViewer.cpp
#include <windows.h>
#include "KWnd.h"
#include "KDib.h"

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

		wd = ws = bmp.GetWidth();
		hd = hs = bmp.GetHeight();
		bmp.Draw(hDC, 0, 0, wd, hd, 0, 0, ws, hs, SRCCOPY);

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
