//////////////////////////////////////////////////////////////////////
// CreateMyProcess.cpp
#include <windows.h>
#include "resource.h"
#include "KWnd.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//====================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	KWnd mainWnd("CreateMyProcess", hInstance, nCmdShow, WndProc,
		MAKEINTRESOURCE(IDR_MENU1), 100, 100, 400, 300);	

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

	STARTUPINFO si;
	static PROCESS_INFORMATION pi;
	BOOL success;

	switch (uMsg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_CREATE_PROCESS:
			ZeroMemory( &si, sizeof(si) );
			si.cb = sizeof(si);
			success = CreateProcess( NULL, "calc.exe", NULL, NULL, FALSE,
				0, NULL, NULL, &si, &pi);
			if (!success)
				MessageBox(hWnd, "Error of CreateProcess", NULL, MB_OK);
			break;

		default:
			break;
		}

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

    case WM_DESTROY:
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////
