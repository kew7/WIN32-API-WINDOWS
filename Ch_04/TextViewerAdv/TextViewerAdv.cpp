//////////////////////////////////////////////////////////////////////
// TextViewerAdv.cpp
#include <windows.h>
#include <zmouse.h>    // для обработки колесика мыши

#include "KWnd.h"
#include "KDocument.h"

#define FILE_NAME "c:\\Program Files\\Microsoft SDKs\\Azure\\.NET SDK\\v2.9\\ref\\\README.TXT"
//#define FILE_NAME "C:\\Program files\\Microsoft Visual Studio\\VC98\\MFC\\SRC\\README.TXT"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

KDocument doc;
//====================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	if (!doc.Open(FILE_NAME))
		return 0;

	KWnd mainWnd("Text Viewer Advanced", hInstance, nCmdShow, WndProc, NULL,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, CS_HREDRAW | CS_VREDRAW,
		WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL);	

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
	TEXTMETRIC tm;
	int cxClient=0, cyClient=0;
	static int xInc, yInc;
	short status;

	switch (uMsg)
	{
	case WM_CREATE:
		hDC = GetDC(hWnd);
		GetTextMetrics(hDC, &tm);
		doc.Initialize(&tm); 
		ReleaseDC(hWnd, hDC);
		break;

	case WM_SIZE:
		hDC = GetDC(hWnd);
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		if (cxClient > 0)
			doc.ScrollSettings(hWnd, cxClient, cyClient);

		ReleaseDC(hWnd, hDC);
		break;

	case WM_VSCROLL:
		switch(LOWORD(wParam)) {
		case SB_LINEUP:
			yInc = -1;  break;
		case SB_LINEDOWN:
			yInc = 1;  break;
		case SB_PAGEUP:
			yInc = -(int)doc.vsi.nPage;  break;
		case SB_PAGEDOWN:
			yInc = (int)doc.vsi.nPage;  break;
		case SB_THUMBTRACK:
			yInc = HIWORD(wParam) - doc.vsi.nPos;  break;

		case SB_TOP:
			yInc = -doc.vsi.nPos;  break;
		case SB_BOTTOM:
			yInc = doc.vsi.nMax - doc.vsi.nPos;    break;
		default:
			yInc = 0;
		}
		doc.UpdateVscroll(hWnd, yInc);
		break;

	case WM_HSCROLL:
		switch(LOWORD(wParam)) {
		case SB_LINELEFT:
			xInc = -1;  break;
		case SB_LINERIGHT:
			xInc = 1;  break;
		case SB_PAGELEFT:
			xInc = -0.8 * (int)doc.hsi.nPage;  break;
		case SB_PAGERIGHT:
			xInc = 0.8 * (int)doc.hsi.nPage;  break;
		case SB_THUMBTRACK:
			xInc = HIWORD(wParam) - doc.hsi.nPos;  break;

		case SB_TOP:
			xInc = -doc.hsi.nPos;  break;
		case SB_BOTTOM:
			xInc = doc.hsi.nMax - doc.hsi.nPos;  break;
		default:
			xInc = 0;
		}
		doc.UpdateHscroll(hWnd, xInc);
		break;

	case WM_MOUSEWHEEL:

		if (LOWORD(wParam) & MK_SHIFT) {
			xInc = -3 * (short)HIWORD(wParam) / WHEEL_DELTA;
			doc.UpdateHscroll(hWnd, xInc);
		}
		else {
			yInc = -3 * (short)HIWORD(wParam) / WHEEL_DELTA;
			doc.UpdateVscroll(hWnd, yInc);
		}
		break;

	case WM_KEYDOWN:
		switch (wParam) {
		case VK_UP:    SendMessage(hWnd, WM_VSCROLL, SB_LINEUP, 0);	
			break;
		case VK_DOWN:  SendMessage(hWnd, WM_VSCROLL, SB_LINEDOWN, 0);	
			break;

		case VK_LEFT:  SendMessage(hWnd, WM_HSCROLL, SB_LINELEFT, 0);	
			break;
		case VK_RIGHT: SendMessage(hWnd, WM_HSCROLL, SB_LINERIGHT, 0);	
			break;

		case VK_PRIOR: SendMessage(hWnd, WM_VSCROLL, SB_PAGEUP, 0);	
			break;
		case VK_NEXT:  SendMessage(hWnd, WM_VSCROLL, SB_PAGEDOWN, 0);	
			break;

		case VK_HOME:
			status = GetKeyState(VK_CONTROL);
			if (0x80 & status) SendMessage(hWnd, WM_VSCROLL, SB_TOP, 0);
			else               SendMessage(hWnd, WM_HSCROLL, SB_TOP, 0);
			break;
		case VK_END:
			status = GetKeyState(VK_CONTROL);
			if (0x80 & status) SendMessage(hWnd, WM_VSCROLL, SB_BOTTOM, 0);
			else               SendMessage(hWnd, WM_HSCROLL, SB_BOTTOM, 0);
			break;
		}
		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);

		doc.PutText(hWnd, hDC);

		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return (DefWindowProc(hWnd, uMsg, wParam, lParam));
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////
