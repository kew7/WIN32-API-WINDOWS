//////////////////////////////////////////////////////////////////////
// PlayMetaFile.cpp

#include <windows.h>
#include "KWnd.h"

#define FILE_NAME "Pict1.emf"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//====================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	KWnd mainWnd("PlayMetaFile", hInstance, nCmdShow, WndProc);	

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
	HENHMETAFILE hemf;
	ENHMETAHEADER emh;
	int x1, y1, x2, y2;

	switch (uMsg)
	{

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);

		hemf = GetEnhMetaFile(FILE_NAME);
		if (!hemf)
			MessageBox(hWnd, "Файл " FILE_NAME " не найден.", "Error", MB_OK);
		else {
			GetEnhMetaFileHeader(hemf, sizeof(ENHMETAHEADER), &emh);
			x1 = emh.rclBounds.left;  y1 = emh.rclBounds.top;
			x2 = emh.rclBounds.right; y2 = emh.rclBounds.bottom;

			SetRect(&rect, x1, y1, x2, y2);

			PlayEnhMetaFile(hDC, hemf, &rect);
			DeleteEnhMetaFile(hemf);
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
