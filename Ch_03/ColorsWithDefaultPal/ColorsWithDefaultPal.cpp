//////////////////////////////////////////////////////////////////////
// ColorsWithDefaultPal.cpp

#include <windows.h>
#include "KWnd.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//====================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	KWnd mainWnd("Colors with default palette", hInstance, nCmdShow, WndProc, NULL, 0, 0, 800, 200);	

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
	static HBRUSH brush[8];

	COLORREF rgbColor[8] = { RGB(128,0,0), RGB(192,0,0),RGB(255,0,0),
						RGB(255,192,0), RGB(176,250,133), RGB(245,197,137),
						RGB(255,128,128), RGB(255,128,255)};
	switch (uMsg)
	{
	case WM_CREATE:
		for (i = 0; i < 8; ++i)
			brush[i] = CreateSolidBrush(rgbColor[i]);
		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
	
		GetClientRect(hWnd, &r0);
		dW = r0.right / 8;
		dH = r0.bottom;
		for (i = 0; i < 8; ++i) {
			SetRect(&rect[i], i*dW, 0, (i+1)*dW, dH);
			FillRect(hDC, &rect[i], brush[i]);
		}
	
		EndPaint(hWnd, &ps);
		break;

    case WM_DESTROY:
		for (i = 0; i < 8; ++i)
			DeleteObject(brush[i]);

		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}
