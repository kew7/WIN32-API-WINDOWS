//////////////////////////////////////////////////////////////////////
// ScribbleAdv.cpp
#include <windows.h>
#include <vector>
using namespace std; 

#include "KWnd.h"

struct Point {
	Point(int _x = 0, int _y = 0) : x(_x), y(_y) {}
	int x;
	int y;
};

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//====================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	KWnd mainWnd("ScribbleAdvanced", hInstance, nCmdShow, WndProc);	

	while (GetMessage(&msg, NULL, 0, 0))  {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

//====================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HDC hDC; // контекст устройства для рисования мышью
	HDC hdc;     // контекст устройства для восстановления
	             // рисунка при обработке WM_PAINT
	PAINTSTRUCT ps;
	static int x, y;  // позиция курсора мыши
	static BOOL bTracking = FALSE;

	static vector<Point> curve;
	static vector<vector<Point> > curves;
	vector<Point>::iterator it;

	int i, j;

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
		curve.push_back(Point(x, y));
		break; 
 
	case WM_LBUTTONUP:
		if (bTracking) {
			bTracking = FALSE; 
			curves.push_back(curve);
			curve.clear();
		}
		break; 
 
	case WM_MOUSEMOVE: 
		if (bTracking) {
			// новая позиция
			x = LOWORD(lParam);
			y = HIWORD(lParam);
			LineTo(hDC, x, y);
			curve.push_back(Point(x, y));
		} 
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		for (i = 0; i < curves.size(); ++i) {
			it = curves[i].begin();
			MoveToEx(hdc, it->x, it->y, NULL);
			for (it + 1; it != curves[i].end(); ++it)
				LineTo(hdc, it->x, it->y);
		}
		EndPaint(hWnd, &ps);
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
