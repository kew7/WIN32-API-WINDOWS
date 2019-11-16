//////////////////////////////////////////////////////////////////////
// ElasticRect.cpp
#include <windows.h>
#include "KWnd.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//====================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	KWnd mainWnd("Elastic Rectangle", hInstance, nCmdShow, WndProc);	

	while (GetMessage(&msg, NULL, 0, 0))  {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

//====================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT style;
	static HDC hDC;
	static	int x1, y1, x2, y2;
	static BOOL bTracking = FALSE;

	static HBRUSH hOldBrush;
	static HPEN hDotPen, hOldPen;

	switch (uMsg)
	{
	case WM_CREATE:
		style = GetClassLong(hWnd, GCL_STYLE);
		SetClassLong(hWnd, GCL_STYLE, style | CS_DBLCLKS);

		hDotPen = CreatePen(PS_DOT, 1, RGB(0,0,0));
		hDC = GetDC(hWnd);
		hOldBrush = (HBRUSH)SelectObject(hDC, GetStockObject(HOLLOW_BRUSH));
		break;

	case WM_LBUTTONDOWN: 
		bTracking = TRUE;
		SetROP2(hDC, R2_NOTXORPEN);
		x1 = x2 = LOWORD(lParam);
		y1 = y2 = HIWORD(lParam);

		hOldPen = (HPEN)SelectObject(hDC, hDotPen);
		Rectangle(hDC, x1, y1, x2, y2);
		break; 
 
	case WM_LBUTTONUP:
		if (bTracking) {
			bTracking = FALSE; 
			SetROP2(hDC, R2_COPYPEN);
			// нарисовать окончательный прямоугольник
			x2 = LOWORD(lParam);
			y2 = HIWORD(lParam);
			SelectObject(hDC, hOldPen);
			Rectangle(hDC, x1, y1, x2, y2);			
		}
		break; 
 
	case WM_MOUSEMOVE: 
		if (bTracking) {
			// стереть предшествующий прямоугольник
			Rectangle(hDC, x1, y1, x2, y2);

			// нарисовать новый прямоугольник
			x2 = LOWORD(lParam);
			y2 = HIWORD(lParam);
			Rectangle(hDC, x1, y1, x2, y2);
		} 
		break;

	case WM_LBUTTONDBLCLK: 
		bTracking = FALSE; 
		x1 = LOWORD(lParam);
		y1 = HIWORD(lParam);
		Rectangle(hDC, x1, y1, x1 + 100, y1 + 100);		
		break; 
 
    case WM_DESTROY:
		SelectObject(hDC, hOldBrush);
		ReleaseDC(hWnd, hDC);
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

