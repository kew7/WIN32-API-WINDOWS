//////////////////////////////////////////////////////////////////////
// CommonDialogs.cpp
#include <windows.h>
#include <stdio.h>
#include "KWnd.h"
#include "resource.h"

#define ESC_OF "Отказ от выбора или ошибка выполнения функции "

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//==================================================================== 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	KWnd mainWnd("CommonDialogs", hInstance, nCmdShow, WndProc,
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
	RECT rcClient;
	BOOL success;
	static COLORREF textColor;

	// Переменные для стандартных диалогов "Open", "Save as"
	static OPENFILENAME ofn;
	static char szFile[MAX_PATH];
	
	// Переменные для стандартного диалога "Color"
	static CHOOSECOLOR cc;          // common dialog box structure 
	static COLORREF acrCustClr[16]; // array of custom colors 

	// Переменные для стандартного диалога "Font"
	static CHOOSEFONT chf;
	static HFONT hFont;
	static LOGFONT lf;

	switch (uMsg)
	{
	case WM_CREATE:
		// Инициализация структуры ofn
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = hWnd;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);

		// Инициализация структуры cc
		cc.lStructSize = sizeof(CHOOSECOLOR);
		cc.hwndOwner = hWnd;
		cc.lpCustColors = (LPDWORD) acrCustClr;
		cc.Flags = CC_FULLOPEN | CC_RGBINIT;

		// Инициализация структуры chf
		chf.lStructSize = sizeof(CHOOSEFONT);
		chf.hwndOwner = hWnd;
		chf.lpLogFont = &lf;
		chf.Flags = CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT;
		chf.nFontType = SIMULATED_FONTTYPE;
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_OPEN:	
			strcpy(szFile, "");
			success = GetOpenFileName(&ofn);
			if (success)
				MessageBox(hWnd, ofn.lpstrFile, "Открывается файл:", MB_OK);
			else
				MessageBox(hWnd, ESC_OF"GetOpenFileName",
				"Отказ от выбора или ошибка", MB_ICONWARNING);
			break;

		case IDM_SAVE_AS:
			strcpy(szFile, "");
			success = GetSaveFileName(&ofn);
			if (success)
				MessageBox(hWnd, ofn.lpstrFile,
				"Файл сохраняется с именем:", MB_OK);
			else
				MessageBox(hWnd, ESC_OF"GetSaveFileName",
				"Отказ от выбора или ошибка", MB_ICONWARNING);
			break;

		case IDM_BKGR_COLOR:
 			if (ChooseColor(&cc))
				SetClassLong(hWnd, GCL_HBRBACKGROUND,
				(LONG)CreateSolidBrush(cc.rgbResult));
			break;
		
		case IDM_TEXT_COLOR:
 			if (ChooseColor(&cc))  textColor = cc.rgbResult;
			break;

		case IDM_CHOOSE_FONT:
			if(ChooseFont(&chf))  hFont = CreateFontIndirect(chf.lpLogFont);	
			break;

		case IDM_EXIT:
			SendMessage(hWnd, WM_DESTROY, 0, 0);
			break;

		default:
			break;
		}
		InvalidateRect(hWnd, NULL, TRUE);		
		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		SetBkMode(hDC, TRANSPARENT);
		SetTextColor(hDC, textColor);
		if (hFont) 
			DeleteObject(SelectObject(hDC, hFont));

		GetClientRect(hWnd, &rcClient);
		DrawText(hDC, "Common Dialogs", -1, &rcClient,
			DT_SINGLELINE | DT_CENTER | DT_VCENTER);
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
