//////////////////////////////////////////////////////////////////////
// DrawIconWithRop.cpp
#include <windows.h>
#include "KWnd.h"

#define ICON_ID  12

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//====================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	KWnd mainWnd("Draw icon with ROP", hInstance, nCmdShow, WndProc);	

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

	static HINSTANCE hShell32;
	ICONINFO iconInfo;
	BITMAP bmp;
	HICON hIcon;
	HDC hMemDC;
	int x = 20, y = 20;
	
	switch (uMsg)
	{

	case WM_CREATE:

		hShell32 = LoadLibrary("Shell32.dll");
		SetClassLong(hWnd, GCL_HBRBACKGROUND, (LONG)CreateSolidBrush(RGB(180,140,200)));

		break;


	case WM_PAINT:

		hDC = BeginPaint(hWnd, &ps);
		hMemDC = CreateCompatibleDC(hDC);
		hIcon = (HICON)LoadImage(hShell32, MAKEINTRESOURCE(ICON_ID),
			IMAGE_ICON, 48, 48, LR_DEFAULTCOLOR);

		if (!hIcon) {
			MessageBox(hWnd, "Ошибка загрузки пиктограммы", "Error", MB_OK);
			break;
		}
		
				
		GetIconInfo(hIcon, &iconInfo);

		GetObject(iconInfo.hbmMask, sizeof(bmp), &bmp);

		// Маска
		SelectObject(hMemDC, iconInfo.hbmMask);
		BitBlt(hDC, x, y, bmp.bmWidth, bmp.bmHeight, hMemDC, 0, 0, SRCCOPY);

		// Изображение
		SelectObject(hMemDC, iconInfo.hbmColor);
		BitBlt(hDC, x+60, y, bmp.bmWidth, bmp.bmHeight, hMemDC, 0, 0, SRCCOPY);

		// Вывод пиктограммы наложением маски и рисунка
		SelectObject(hMemDC, iconInfo.hbmMask);
		BitBlt(hDC, x+120, y, bmp.bmWidth, bmp.bmHeight, hMemDC, 0, 0, SRCAND);
		SelectObject(hMemDC, iconInfo.hbmColor);
		BitBlt(hDC, x+120, y, bmp.bmWidth, bmp.bmHeight, hMemDC, 0, 0, SRCINVERT);		

		// Вывод пиктограммы функцией DrawIcon
		DrawIcon(hDC, x+188, y, hIcon); 

		DeleteObject(hMemDC);
		DestroyIcon(hIcon);
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
