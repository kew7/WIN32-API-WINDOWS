//////////////////////////////////////////////////////////////////////
// Scribble.cpp
#include <windows.h>
#include "KWnd.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//====================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	KWnd mainWnd("Scribble", hInstance, nCmdShow, WndProc);	

	while (GetMessage(&msg, NULL, 0, 0))  {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

//====================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HDC hDC;
	static int x, y;  // позиция курсора мыши
	static BOOL bTracking = FALSE; 

	switch (uMsg)
	{
	case WM_CREATE:
		hDC = GetDC(hWnd);
		break;

	case WM_LBUTTONDOWN: 
		bTracking = TRUE;
		// начальная позиция
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		MoveToEx(hDC, x, y, NULL);
		break; 
 
	case WM_LBUTTONUP:
		if (bTracking)
			bTracking = FALSE; 
		break; 
 
	case WM_MOUSEMOVE: 
		if (bTracking) {
			// новая позиция
			x = LOWORD(lParam);
			y = HIWORD(lParam);
			LineTo(hDC, x, y);
		} 
		break;

    case WM_DESTROY:	
		ReleaseDC(hWnd, hDC); 
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////
