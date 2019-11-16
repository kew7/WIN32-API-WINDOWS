//////////////////////////////////////////////////////////////////////
// � ���� ��������� ..........
//       ����� �� ����������� ��� � ���������� ��������, ��� 
// ������ ���� ������������ ������ ���������, � �������� ��������������� ������� ����.
//
//  ��������� ������� ����������� ��� ���������� �������� �������� ����.
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
	static FLOAT x = 0, y = 0; // ������� ������� ����
	static FLOAT dX, dY; // ���������� ��������� ��� ������ �� ����� �������
	static int alpha = 0; // ����, ������������ �������� ����

	static HDC hDC;          // �������� �������
	static HBRUSH hBkBrush;  // ������� �����
	HBITMAP hBmpBkgr;        // ����� ��� ������� �����
	static HBITMAP hBmpBall; // ����� � ������������ ����
	static BITMAP bm;        // ��������� ������ 
	RECT rect;  // ������������� ���������� �������
	RECT rBall; // �������������, �������������� ����������� ����
	
	static HDC hMemDcFrame;   // ����������� �������� (�������� � ������)
	static HBITMAP hBmpFrame; // ����� ��� ������������ ���������
	static int count = 0;

	switch(uMsg)
	{
	case WM_CREATE:
		hDC = GetDC(hWnd);
		GetClientRect(hWnd, &rect);
		dX = rect.right / 100.;
		dY = rect.bottom / 50.;
		// ������� ������ (0.1 ���)
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
		// ����������� ���� � hMemDcFrame
		BitBlt(hMemDcFrame, 0, 0, rect.right, rect.bottom, hDC, 0, 0, SRCCOPY);
		break;
	
	case WM_TIMER:
		GetClientRect(hWnd, &rect);
		if (!count) {    // ����������� ���� � hMemDcFrame
			BitBlt(hMemDcFrame, 0, 0, rect.right, rect.bottom, hDC, 0, 0, SRCCOPY);
			count++;
		}
		// ������� ������� �������� ����
		SetRect(&rBall, x, y, x + bm.bmWidth, y + bm.bmHeight);
		FillRect(hMemDcFrame, &rBall, hBkBrush);

		// ����� ������� ����
		x += dX;
		y += dY;
		alpha += 10;
		if (alpha > 360)  alpha = 0;

		// ���� ��� ������ ���� ����, ����������� ��� �������� ����������
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
	
	// ���������� � ������ ����
	HDC hMemDcBall = CreateCompatibleDC(hdc);
	SelectObject(hMemDcBall, hBmp);

	// ������� ������ ���������
	hRgn = CreateEllipticRgn(x, y, x + bm.bmWidth, y + bm.bmHeight);
	SelectClipRgn(hMemFrameDC, hRgn);

	// ������� �������������� ��� ����������� � �������� ����
	xform.eM11 = (FLOAT) cos(alpha * 2 * Pi / 360);  //��������
	xform.eM12 = (FLOAT) sin(alpha * 2 * Pi / 360);  //��������
	xform.eM21 = (FLOAT) -sin(alpha * 2 * Pi / 360); //��������
	xform.eM22 = (FLOAT) cos(alpha * 2 * Pi / 360);  //��������
	xform.eDx  = x + bm.bmWidth / 2;        //�������� �� ��� x
	xform.eDy  = y + bm.bmHeight / 2;       //�������� �� ��� y

	// ����� ���� � �������� hMemFrameDC
	SaveDC(hMemFrameDC);			
	BOOL ret = SetWorldTransform(hMemFrameDC, &xform);
	BitBlt(hMemFrameDC, -bm.bmWidth/2, -bm.bmHeight/2, 
			bm.bmWidth, bm.bmHeight, hMemDcBall, 0, 0, SRCCOPY);
	RestoreDC(hMemFrameDC, -1);
	
	// ����������� ����������� �� hMemFrameDC � hdc
	RECT rect;
	GetClientRect(hwnd, &rect);
	BitBlt(hdc, 0, 0, rect.right, rect.bottom, hMemFrameDC, 0, 0, SRCCOPY);

	SelectClipRgn(hMemFrameDC, NULL);
	DeleteObject(hRgn);
	DeleteDC(hMemDcBall);
}
//////////////////////////////////////////////////////////////////////
