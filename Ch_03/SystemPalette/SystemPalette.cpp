//////////////////////////////////////////////////////////////////////
// SystemPalette.cpp

#include <windows.h>
#include "KWnd.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	KWnd mainWnd("System palette", hInstance, nCmdShow, WndProc, NULL, 0, 0, 524, 543);	

	while (GetMessage(&msg, NULL, 0, 0))  {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

//////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hDC;
	PAINTSTRUCT ps;
	RECT r0;
	RECT rect;

	HBRUSH brush;
	int dW, dH, i, j;
	int index = 0;
		
	PALETTEENTRY pal[256];

	switch (uMsg)
	{
	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		
		GetSystemPaletteEntries(hDC, 0, 256, pal);

		GetClientRect(hWnd, &r0);
		dW = (r0.right - 4) / 16;
		dH = (r0.bottom - 4) / 16;
		SetViewportOrgEx(hDC, 2, 2, NULL);

		for (i = 0; i < 16; ++i)
			for (j = 0; j < 16; ++j) {
				SetRect(&rect, j*dW, i*dH, (j+1)*dW - 1, (i+1)*dH - 1);
				brush = CreateSolidBrush(RGB(pal[index].peRed,
					    pal[index].peGreen, pal[index].peBlue));
				FillRect(hDC, &rect, brush);
				DeleteObject(brush);
				index++;
			}
	
		EndPaint(hWnd, &ps);
		break;
	
	case WM_PALETTECHANGED:
		InvalidateRect(hWnd, NULL, TRUE);
		break;

    case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

