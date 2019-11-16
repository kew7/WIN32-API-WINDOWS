//////////////////////////////////////////////////////////////////////
// BadCount.cpp
#include <windows.h>
#include <stdio.h>
#include "KWnd.h"

#define N  50000000
long g_counter = 0;

DWORD WINAPI ThreadFunc(LPVOID);
void IncCounter();
void DecCounter();
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//====================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	KWnd mainWnd("BadCount", hInstance, nCmdShow, WndProc,
		NULL, 100, 100, 400, 100);	

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
	HANDLE hThread;
	char text[100];

	switch (uMsg)
	{
	case WM_CREATE:
		hThread = CreateThread(NULL, 0, ThreadFunc, NULL, 0, NULL);
		if (!hThread)
			MessageBox(hWnd, "Error of CreateThread", NULL, MB_OK);

		DecCounter();

		WaitForSingleObject(hThread, INFINITE);
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		sprintf(text, "g_counter = %d", g_counter);
		TextOut(hDC, 20, 20, text, strlen(text));
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
//====================================================================
DWORD WINAPI ThreadFunc(LPVOID lpv)
{
	IncCounter();
	return 0;
}
//====================================================================
void IncCounter()
{
	for (int i = 0; i < N; ++i)    ++g_counter;
}
//====================================================================
void DecCounter()
{
	for (int i = 0; i < N; ++i)    --g_counter;
}
//////////////////////////////////////////////////////////////////////

