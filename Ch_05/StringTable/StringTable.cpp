//////////////////////////////////////////////////////////////////////
// StringTable.cpp
#include <windows.h>
#include <stdio.h>
#include "KWnd.h"
#include "resource.h"

char szAppName[] = "StringTable";

int StatusFileMsg(HWND hwnd, int status, char* fileName);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//==================================================================== 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;

	KWnd mainWnd("StringTable", hInstance, nCmdShow, WndProc);	

	while (GetMessage(&msg, NULL, 0, 0))  {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
//==================================================================== 
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		StatusFileMsg(hWnd, IDS_FILEREADONLY, "File_1");
		StatusFileMsg(hWnd, IDS_FILETOOBIG, "File_2");
		StatusFileMsg(hWnd, IDS_FILENOTFOUND, "File_3");
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
int StatusFileMsg(HWND hwnd, int status, char* fileName)
{
	HINSTANCE hInst;
	char szFormat[80];
	char szBuffer[200];

	hInst = GetModuleHandle(NULL);
	LoadString(hInst, status, szFormat, 80);
	sprintf(szBuffer, szFormat, fileName);
	
	return MessageBox(hwnd, szBuffer, szAppName, MB_OK | MB_ICONEXCLAMATION);
}
