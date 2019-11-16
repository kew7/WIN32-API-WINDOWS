//////////////////////////////////////////////////////////////////////
// TextJust.cpp
#include <windows.h>
#include "KWnd.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL TextJustOut(HDC hdc, int x, int y, LPCTSTR lpStr, int width, char breakChar = ' ');

//////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	KWnd mainWnd("Text Justification", hInstance, nCmdShow, WndProc);	

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

	char* text[6] =
		{ "Рассуждай",
		  "Рассуждай токмо",
		  "Рассуждай токмо о том",
		  "Рассуждай токмо о том, о чем понятия",
		  "Рассуждай токмо о том, о чем понятия твои тебе",
		  "Рассуждай токмо о том, о чем понятия твои тебе сие дозволяют."
		};

	int x = 20, y = 20;
	SIZE size;
	int nWidth, i;

	switch (uMsg)
	{
	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);

		GetTextExtentPoint32(hDC, text[5], strlen(text[5]), &size);
		nWidth = size.cx;
		for (i = 0; i < 6; i++) {
			TextJustOut(hDC, x, y, text[i], nWidth);
			y += size.cy + 2;
		}

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
BOOL TextJustOut(HDC hdc, int x, int y, LPCTSTR lpStr, int width, TCHAR breakChar)
{
	SIZE size;
	int nCount = strlen(lpStr);

	SetTextJustification(hdc, 0, 0);
	GetTextExtentPoint32(hdc, lpStr, nCount, &size);

	int nBreak = 0;
	for (int i = 0; i < nCount; ++i)
		if (lpStr[i] == breakChar)
			nBreak++;
	int breakExtra = width - size.cx;
	if (breakExtra < 0)  breakExtra = 0;
	SetTextJustification(hdc, breakExtra, nBreak);
	return TextOut(hdc, x, y, lpStr, nCount);
}
