//////////////////////////////////////////////////////////////////////
// CreateMetaFile.cpp

#include <windows.h>
#include "KWnd.h"

#define FILE_NAME "Pict1.emf"

void DrawSomething(HDC);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//====================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	KWnd mainWnd("CreateMetaFile", hInstance, nCmdShow, WndProc);	

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
	static HDC hdcEMF;
	HENHMETAFILE hemf;

	switch (uMsg)
	{

	case WM_CREATE:
		
		hdcEMF = CreateEnhMetaFile(NULL, FILE_NAME, NULL, "CreateMetaFile\0Pict1\0");

		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);

		DrawSomething(hDC);
		DrawSomething(hdcEMF);

		EndPaint(hWnd, &ps);
		break;

    case WM_DESTROY:
		hemf = CloseEnhMetaFile(hdcEMF);
		DeleteEnhMetaFile(hemf);
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}
//////////////////////////////////////////////////////////////////////

void DrawSomething(HDC hdc) {

	RECT r;
	HPEN hPen, hOldPen;
	HBRUSH hBrush, hOldBrush;

	SetRect(&r, 20, 20, 220, 220);

	hPen = CreatePen(PS_SOLID, 10, RGB(255, 160, 140));
	hOldPen = (HPEN)SelectObject(hdc, hPen);
	
	hBrush = CreateSolidBrush(RGB(140, 160, 255));
	hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
	Rectangle(hdc, r.left, r.top, r.right, r.bottom);

	hPen = CreatePen(PS_SOLID, 3, RGB(0, 255, 0));
	SelectObject(hdc, hPen);
	hBrush = CreateSolidBrush(RGB(100, 100, 100));
	SelectObject(hdc, hBrush);
	InflateRect(&r, -40, -40);
	Ellipse(hdc, r.left, r.top, r.right, r.bottom);
	SetTextColor(hdc, RGB(255, 255, 255));
	SetBkMode(hdc, TRANSPARENT);
	DrawText(hdc, "YES !", -1, &r, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	
	DeleteObject(SelectObject(hdc, hOldPen));
	DeleteObject(SelectObject(hdc, hOldBrush));
}