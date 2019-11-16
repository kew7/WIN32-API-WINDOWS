//////////////////////////////////////////////////////////////////////
// ¬ этой программе ...
//       чтобы не затрагивать фон с текстурным рисунком, при 
// выводе м€ча используетс€ регион отсечени€, в точности соответствующий контуру м€ча.
//
//////////////////////////////////////////////////////////////////////
// BounceBall.cpp
#include <windows.h>
#include <stdio.h>
#include <math.h>
#include "KWnd.h"
#include "resource.h"

#define Pi 3.14159265

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void DrawBall(HWND, HDC, HBITMAP, BITMAP, FLOAT, FLOAT, int);
//====================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	KWnd mainWnd("Bounce ball", hInstance, nCmdShow, WndProc);	

	while (GetMessage(&msg, NULL, 0, 0))  {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

//====================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                     LPARAM lParam)
{
	static FLOAT x = 0, y = 0; // текуща€ позици€ м€ча
	static FLOAT dX, dY; // приращени€ координат дл€ сдвига на новую позицию
	static int alpha = 0; // угол, определ€ющий вращение м€ча

	static HDC hDC;          // контекст диспле€
	static HBRUSH hBkBrush;  // узорна€ кисть
	HBITMAP hBmpBkgr;        // растр дл€ узорной кисти
	static HBITMAP hBmpBall; // растр с изображением м€ча
	static BITMAP bm;        // параметры растра 
	RECT rect;  // пр€моугольник клиентской области
	RECT rBall; // пр€моугольник, ограничивающий изображение м€ча

	switch(uMsg)
	{
	case WM_CREATE:
		hDC = GetDC(hWnd);
		GetClientRect(hWnd, &rect);
		dX = rect.right / 100.;
		dY = rect.bottom / 50.;
		// —оздать таймер (0.1 сек)
		SetTimer(hWnd, 1, 100, NULL);

		hBmpBkgr = LoadBitmap((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
					MAKEINTRESOURCE(IDB_STONE));
		hBkBrush = CreatePatternBrush(hBmpBkgr);
		SetClassLong(hWnd, GCL_HBRBACKGROUND, (LONG)hBkBrush);

		hBmpBall = LoadBitmap((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
					MAKEINTRESOURCE(IDB_BALL));
		GetObject(hBmpBall, sizeof(bm), (LPSTR)&bm);

		SetGraphicsMode(hDC, GM_ADVANCED);
		break;
     
	case WM_TIMER:
		GetClientRect(hWnd, &rect);
		// —тираем прежнюю картинку м€ча
		SetRect(&rBall, (int)x, (int)y, (int)x + bm.bmWidth,
			(int)y + bm.bmHeight);
		FillRect(hDC, &rBall, hBkBrush);

		// Ќова€ позици€ м€ча
		x += dX;
		y += dY;
		alpha += 10;
		if (alpha > 360)  alpha = 0;

		// ≈сли м€ч достиг кра€ окна, направление его движени€ измен€етс€
		if(x + bm.bmWidth > rect.right || x < 0)
			dX = -dX;
		if(y + bm.bmHeight > rect.bottom || y < 0)
			dY = -dY;

		DrawBall(hWnd, hDC, hBmpBall, bm, x, y, alpha);
		break;

	case WM_DESTROY:
		KillTimer(hWnd, 1);
		ReleaseDC(hWnd, hDC);
		PostQuitMessage(0);	
		break;
     
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
     }
     return 0;
}

//====================================================================
void DrawBall(HWND hwnd, HDC hdc, HBITMAP hBmp, BITMAP bm, FLOAT x, FLOAT y, int alpha) {
	XFORM xform;
	HRGN hRgn;
	
	// ѕодготовка к выводу м€ча
	HDC hBallMemDC = CreateCompatibleDC(hdc);
	SelectObject(hBallMemDC, hBmp);
	
	// —оздаем регион отсечени€
	hRgn = CreateEllipticRgn(x, y, x + bm.bmWidth, y + bm.bmHeight);
	SelectClipRgn(hdc, hRgn);
	
	// ћировые преобразовани€ дл€ перемещени€ и вращени€ м€ча
	xform.eM11 = (FLOAT) cos(alpha * 2 * Pi / 360);	 //вращение
	xform.eM12 = (FLOAT) sin(alpha * 2 * Pi / 360);	 //вращение
	xform.eM21 = (FLOAT) -sin(alpha * 2 * Pi / 360); //вращение
	xform.eM22 = (FLOAT) cos(alpha * 2 * Pi / 360);	 //вращение
	xform.eDx  = x + bm.bmWidth / 2.;		 //смещение по оси x
	xform.eDy  = y + bm.bmHeight / 2.;		 //смещение по оси y

	// ¬ывод м€ча
	SaveDC(hdc);			
	BOOL ret = SetWorldTransform(hdc, &xform);
	BitBlt(hdc, -bm.bmWidth/2, -bm.bmHeight/2, 
			bm.bmWidth, bm.bmHeight, hBallMemDC, 0, 0, SRCCOPY);
	RestoreDC(hdc, -1);

	SelectClipRgn(hdc, NULL);
	DeleteObject(hRgn);
	DeleteDC(hBallMemDC);
}
//////////////////////////////////////////////////////////////////////
