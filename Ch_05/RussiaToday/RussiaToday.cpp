//////////////////////////////////////////////////////////////////////
// RussiaToday.cpp
#include <windows.h>
#include <stdio.h>
#include "KWnd.h"
#include "resource.h"

char szSoundName[] = "MY_SOUND";  // имя звукового ресурса

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
	HDC hDC;
	PAINTSTRUCT ps;
	static HBITMAP hBmpRusFlag;
	HDC hMemDC;
	static BITMAP bm;

	switch (uMsg)
	{
	case WM_CREATE:
		hInst = GetModuleHandle(NULL);
		hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_TRICOLOUR));
		hIconSm = (HICON)LoadImage(hInst, MAKEINTRESOURCE(IDI_TRICOLOUR),
			IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
		SetClassLong(hWnd, GCL_HICON, (LONG)hIcon);
		SetClassLong(hWnd, GCL_HICONSM, (LONG)hIconSm);

		hBmpRusFlag = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_RUSFLAG));
		GetObject(hBmpRusFlag, sizeof(bm), (LPSTR)&bm);

		// Воспроизведение звука из ресурса
		PlaySound (szSoundName, hInst, SND_RESOURCE | SND_ASYNC);
		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		hMemDC = CreateCompatibleDC(hDC);
		SelectObject(hMemDC, hBmpRusFlag);
		BitBlt(hDC, 0, 0, bm.bmWidth, bm.bmHeight, hMemDC, 0, 0, SRCCOPY);
		DeleteDC(hMemDC);
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
