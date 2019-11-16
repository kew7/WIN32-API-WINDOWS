//////////////////////////////////////////////////////////////////////
// В этой программе ..........
//       чтобы не затрагивать фон с текстурным рисунком, при 
// выводе мяча используется регион отсечения, в точности соответствующий контуру мяча.
//
//  Добавлена двойная буферизация для исключения мерцания картинки мяча.
//
//////////////////////////////////////////////////////////////////////
// BounceBallAdv.cpp
#include <windows.h>
#include <stdio.h>
#include <math.h>
#include "KWnd.h"
#include "resource.h"

#define Pi 3.14159265

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void DrawBall(HWND, HDC, HDC, HBITMAP, BITMAP, FLOAT, FLOAT, int);
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
	static FLOAT x = 0, y = 0; // текущая позиция мяча
	static FLOAT dX, dY; // приращения координат для сдвига на новую позицию
	static int alpha = 0; // угол, определяющий вращение мяча

	static HDC hDC;          // контекст дисплея
	static HBRUSH hBkBrush;  // узорная кисть
	HBITMAP hBmpBkgr;        // растр для узорной кисти
	static HBITMAP hBmpBall; // растр с изображением мяча
	static BITMAP bm;        // параметры растра 
	RECT rect;  // прямоугольник клиентской области
	RECT rBall; // прямоугольник, ограничивающий изображение мяча
	
	static HDC hMemDcFrame;   // совместимый контекст (контекст в памяти)
	static HBITMAP hBmpFrame; // растр для совместимого контекста
	static int count = 0;

	switch(uMsg)
	{
	case WM_CREATE:
		hDC = GetDC(hWnd);
		GetClientRect(hWnd, &rect);
		dX = rect.right / 100.;
		dY = rect.bottom / 50.;
		// Создать таймер (0.1 сек)
		SetTimer(hWnd, 1, 100, NULL);

		hBmpBkgr = LoadBitmap((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
					MAKEINTRESOURCE(IDB_STONE));
		hBkBrush = CreatePatternBrush(hBmpBkgr);
		SetClassLong(hWnd, GCL_HBRBACKGROUND, (LONG)hBkBrush);

		hBmpBall = LoadBitmap((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
					MAKEINTRESOURCE(IDB_BALL));
		GetObject(hBmpBall, sizeof(bm), (LPSTR)&bm);

		hMemDcFrame = CreateCompatibleDC(hDC);
		hBmpFrame = CreateCompatibleBitmap(hDC, rect.right, rect.bottom);
		SelectObject(hMemDcFrame, hBmpFrame);		

		SetGraphicsMode(hMemDcFrame, GM_ADVANCED);
		break;

	case WM_SIZE:
		GetClientRect(hWnd, &rect);
		hBmpFrame = CreateCompatibleBitmap(hDC, rect.right, rect.bottom);
		DeleteObject(SelectObject(hMemDcFrame, hBmpFrame));
		// Копирование фона в hMemDcFrame
		BitBlt(hMemDcFrame, 0, 0, rect.right, rect.bottom, hDC, 0, 0, SRCCOPY);
		break;
	
	case WM_TIMER:
		GetClientRect(hWnd, &rect);
		if (!count) {    // Копирование фона в hMemDcFrame
			BitBlt(hMemDcFrame, 0, 0, rect.right, rect.bottom, hDC, 0, 0, SRCCOPY);
			count++;
		}
		// Стираем прежнюю картинку мяча
		SetRect(&rBall, x, y, x + bm.bmWidth, y + bm.bmHeight);
		FillRect(hMemDcFrame, &rBall, hBkBrush);

		// Новая позиция мяча
		x += dX;
		y += dY;
		alpha += 10;
		if (alpha > 360)  alpha = 0;

		// Если мяч достиг края окна, направление его движения изменяется
		if(x + bm.bmWidth > rect.right || x < 0)
			dX = -dX;  
		if(y + bm.bmHeight > rect.bottom || y < 0)
			dY = -dY;

		DrawBall(hWnd, hDC, hMemDcFrame, hBmpBall, bm, (int)x, (int)y, alpha);
		break;

	case WM_DESTROY:
		KillTimer(hWnd, 1);
		ReleaseDC(hWnd, hDC);
		DeleteDC(hMemDcFrame);
		PostQuitMessage(0);	
		break;
     
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
     }
     return 0;
}

//====================================================================
void DrawBall(HWND hwnd, HDC hdc, HDC hMemFrameDC, HBITMAP hBmp,
			  BITMAP bm, FLOAT x, FLOAT y, int alpha) {
	XFORM xform;
	HRGN hRgn;
	
	// Подготовка к выводу мяча
	HDC hMemDcBall = CreateCompatibleDC(hdc);
	SelectObject(hMemDcBall, hBmp);

	// Создаем регион отсечения
	hRgn = CreateEllipticRgn(x, y, x + bm.bmWidth, y + bm.bmHeight);
	SelectClipRgn(hMemFrameDC, hRgn);

	// Мировые преобразования для перемещения и вращения мяча
	xform.eM11 = (FLOAT) cos(alpha * 2 * Pi / 360);  //вращение
	xform.eM12 = (FLOAT) sin(alpha * 2 * Pi / 360);  //вращение
	xform.eM21 = (FLOAT) -sin(alpha * 2 * Pi / 360); //вращение
	xform.eM22 = (FLOAT) cos(alpha * 2 * Pi / 360);  //вращение
	xform.eDx  = x + bm.bmWidth / 2;        //смещение по оси x
	xform.eDy  = y + bm.bmHeight / 2;       //смещение по оси y

	// Вывод мяча в контекст hMemFrameDC
	SaveDC(hMemFrameDC);			
	BOOL ret = SetWorldTransform(hMemFrameDC, &xform);
	BitBlt(hMemFrameDC, -bm.bmWidth/2, -bm.bmHeight/2, 
			bm.bmWidth, bm.bmHeight, hMemDcBall, 0, 0, SRCCOPY);
	RestoreDC(hMemFrameDC, -1);
	
	// Копирование изображения из hMemFrameDC в hdc
	RECT rect;
	GetClientRect(hwnd, &rect);
	BitBlt(hdc, 0, 0, rect.right, rect.bottom, hMemFrameDC, 0, 0, SRCCOPY);

	SelectClipRgn(hMemFrameDC, NULL);
	DeleteObject(hRgn);
	DeleteDC(hMemDcBall);
}
//////////////////////////////////////////////////////////////////////
