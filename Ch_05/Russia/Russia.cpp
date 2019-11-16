//////////////////////////////////////////////////////////////////////
// Russia.cpp
#include <windows.h>
#include <stdio.h>
#include "KWnd.h"
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//==================================================================== 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	KWnd mainWnd("Russia today", hInstance, nCmdShow, WndProc, NULL, 0, 0, 400, 300);	

	while (GetMessage(&msg, NULL, 0, 0))  {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
//==================================================================== 
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HINSTANCE hInst;
	HICON hIcon, hIconSm;
	static HCURSOR hCursor, hCursorUp, hCursorDown;
	static int xPos, yPos;  // координаты горячей точки курсора мыши
	static int wClient, hClient; // размеры клиентской области окна
	HDC hDC;
	PAINTSTRUCT ps;
	char text[100];
	RECT rect;
	static HBITMAP hBmpRusFlag; // дескриптор растра
	HDC hMemDC;      // контекст устройства в памяти
	static BITMAP bm;   // параметры растра 

	SetRect(&rect, 10, 0, 200, 30);

	switch (uMsg)
	{
	case WM_CREATE:
		hInst = GetModuleHandle(NULL);
		hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_TRICOLOUR));
		hIconSm = (HICON)LoadImage(hInst, MAKEINTRESOURCE(IDI_TRICOLOUR),
			IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
		SetClassLong(hWnd, GCL_HICON, (LONG)hIcon);
		SetClassLong(hWnd, GCL_HICONSM, (LONG)hIconSm);

		hCursor = LoadCursor(NULL, IDC_CROSS);
		SetClassLong(hWnd, GCL_HCURSOR, (LONG)hCursor);
		hCursorUp = LoadCursor(hInst, MAKEINTRESOURCE(IDC_UP));
		hCursorDown = LoadCursor(hInst, MAKEINTRESOURCE(IDC_DOWN));
		hBmpRusFlag = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_RUSFLAG));
		GetObject(hBmpRusFlag, sizeof(bm), (LPSTR)&bm);
		break;

	case WM_SIZE:
		wClient = LOWORD(lParam); 
		hClient = HIWORD(lParam); 
		break;

	case WM_MOUSEMOVE:
		xPos = LOWORD(lParam); 
		yPos = HIWORD(lParam);
		if (yPos < 16)
			SetCursor(hCursorUp);
		if (yPos > hClient - 16)
			SetCursor(hCursorDown);
		InvalidateRect(hWnd, &rect, TRUE);		
		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);

		hMemDC = CreateCompatibleDC(hDC);
		SelectObject(hMemDC, hBmpRusFlag);
		BitBlt(hDC, 0, 0, bm.bmWidth, bm.bmHeight, hMemDC, 0, 0, SRCCOPY);
		DeleteDC(hMemDC);

		sprintf(text, "xPos = %d,  yPos = %d\0", xPos, yPos);
		DrawText(hDC, text, -1, &rect, DT_LEFT);

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
