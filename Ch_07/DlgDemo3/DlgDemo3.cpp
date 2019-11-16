//////////////////////////////////////////////////////////////////////
// DlgDemo3.cpp
#include <windows.h>
#include <stdio.h>
#include "KWnd.h"
#include "resource.h"

#define W  200    // ������ ������
#define H  140    // ������ ������

enum ShapeSize { MAX, MIN };

typedef struct {
	int id_shape;   // ������������� ������
	BOOL fRed;       // ���������� �������� �����
	BOOL fGreen;     // ���������� �������� �����
	BOOL fBlue;      // ���������� ������ �����
	int id_bright;  // ������������� ������� �����
} ShapeData;

ShapeData shapeData;    // ���������� ��������� ��� ������ �������
						// ����� ShapeParamDlgProc � WndProc

BOOL CALLBACK ShapeParamDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//==================================================================== 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	KWnd mainWnd("DlgDemo3", hInstance, nCmdShow, WndProc,
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
	static HINSTANCE hInst;
	HDC hDC;
	PAINTSTRUCT ps;
	RECT rect;
	static HMENU hMenu;  // ���������� �������� ����
	int x0, y0, x1, y1, x2, y2;
	POINT pt[4];
	static ShapeSize shapeSize = MIN;
	static BOOL bShow = TRUE;
	static HBRUSH hBrush, hOldBrush;
	char* itemResizeName[2] = { "Decrease!", "Increase!"};
	int intensity[3] = { 85, 170, 255 }; // ������������� RGB-��������� �����
	int brightness;

	switch (uMsg)
	{
	case WM_CREATE:
		hInst = GetModuleHandle(NULL);
		hMenu = GetMenu(hWnd);
		// IDM_OPEN - ����� ����, ����������� �� ���������
		SetMenuDefaultItem(GetSubMenu(hMenu, 0), IDM_OPEN, FALSE);

		// �������� ������� ������� ���� 
		CheckMenuRadioItem(GetSubMenu(hMenu, 1), IDM_SHOW_SHAPE,
			IDM_HIDE_SHAPE, IDM_SHOW_SHAPE, MF_BYCOMMAND);

		shapeData.id_shape = ID_RECTANGLE;
		shapeData.id_bright = ID_DARK;
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_OPEN:
			MessageBox(hWnd, "������ ����� 'Open'", "���� File", MB_OK);
			break;
		case IDM_CLOSE:
			MessageBox(hWnd, "������ ����� 'Close'", "���� File", MB_OK);
			break;
		case IDM_SAVE:
			MessageBox(hWnd, "������ ����� 'Save'", "���� File", MB_OK);
			break;
		case IDM_EXIT:
			SendMessage(hWnd, WM_DESTROY, 0, 0);
			break;

		case IDM_SHOW_SHAPE:
			bShow = TRUE;
			CheckMenuRadioItem(GetSubMenu(hMenu, 1), IDM_SHOW_SHAPE,
				IDM_HIDE_SHAPE, LOWORD(wParam), MF_BYCOMMAND);
			EnableMenuItem(hMenu, IDM_RESIZE, MF_BYCOMMAND | MFS_ENABLED);
			DrawMenuBar(hWnd);
			break;
		case IDM_HIDE_SHAPE:
			bShow = FALSE;
			CheckMenuRadioItem(GetSubMenu(hMenu, 1), IDM_SHOW_SHAPE,
				IDM_HIDE_SHAPE, LOWORD(wParam), MF_BYCOMMAND);
			EnableMenuItem(hMenu, IDM_RESIZE, MF_BYCOMMAND | MFS_GRAYED);
			DrawMenuBar(hWnd);
			break;

		case IDM_RESIZE:
			shapeSize = (shapeSize == MIN)? MAX : MIN;
			ModifyMenu(hMenu, IDM_RESIZE, MF_BYCOMMAND, IDM_RESIZE,
				itemResizeName[shapeSize]);
			DrawMenuBar(hWnd);
			break;

		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUT), hWnd, AboutDlgProc);  
			break;

		case IDM_DRAW_SHAPE:

			DialogBox(hInst, MAKEINTRESOURCE(IDD_SHAPE_PARAM),
				hWnd, ShapeParamDlgProc);

			break;

		default:
			break;
		}

		InvalidateRect(hWnd, NULL, TRUE);		
		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		brightness = intensity[shapeData.id_bright - ID_DARK];
		
		if (bShow) {
			hBrush = CreateSolidBrush(RGB(
				shapeData.fRed? brightness : 0,
				shapeData.fGreen? brightness : 0,
				shapeData.fBlue? brightness : 0));
			hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);

			// ����������� ������ ������ (x0, y0)
			GetClientRect(hWnd, &rect);
			x0 = rect.right / 2;
			y0 = rect.bottom / 2;
			// ���������� �������������� � �������
			if (shapeSize == MIN) {
				x1 = x0 - W/2;    y1 = y0 - H/2;
				x2 = x0 + W/2;    y2 = y0 + H/2;
			}
			else {
				x1 = y1 = 0;
				x2 = rect.right;  y2 = rect.bottom;
			}
			// ���������� �����
			pt[0].x = (x1 + x2) / 2;  pt[0].y = y1;
			pt[1].x = x2;             pt[1].y = (y1 + y2) / 2;
			pt[2].x = (x1 + x2) / 2;  pt[2].y = y2;
			pt[3].x = x1;             pt[3].y =  (y1 + y2) / 2;

			switch (shapeData.id_shape) {
			case ID_RECTANGLE:
				Rectangle(hDC, x1, y1, x2, y2);
				break;
			case ID_RHOMB:
				Polygon(hDC, pt, 4);
				break;
			case ID_ELLIPSE:
				Ellipse(hDC,  x1, y1, x2, y2);
				break;
			}

			DeleteObject(SelectObject(hDC, hOldBrush));
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

//==================================================================== 
BOOL CALLBACK AboutDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hStatic1;
	static LOGFONT lf;  // ����������� static ��� ������������� ����� ������ !!!
	static HFONT hFont1;

	switch (uMsg) {
	case WM_INITDIALOG:

		hStatic1 = GetDlgItem(hDlg, IDC_STATIC_1);

		// �������� ����������� ������ Verdana
		lf.lfHeight = 28;
		lstrcpy( (LPSTR)&lf.lfFaceName, "Verdana" );
		hFont1 = CreateFontIndirect(&lf);

		// ����������� ������ ��� �������� hStatic1
		SendMessage(hStatic1, WM_SETFONT, (WPARAM)hFont1, TRUE );  

		return TRUE;

	case WM_CTLCOLORSTATIC:

		// ����������� �������� ��������� �������� hStatic1
		if ((HWND)lParam == hStatic1) {
			SetTextColor((HDC)wParam, RGB(160, 0, 0));
			SetBkMode((HDC)wParam, TRANSPARENT);
			return (DWORD)GetSysColorBrush(COLOR_3DFACE);
		}
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
		case IDCANCEL:
			DeleteObject(hFont1);
			EndDialog(hDlg, 0);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

//==================================================================== 
BOOL CALLBACK ShapeParamDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static ShapeData shapeDataNew;

	switch (uMsg) {
	case WM_INITDIALOG:
		CheckRadioButton(hDlg, ID_RECTANGLE, ID_ELLIPSE, shapeData.id_shape);
		CheckRadioButton(hDlg, ID_DARK, ID_LIGHT, shapeData.id_bright);
		CheckDlgButton(hDlg, ID_RED, shapeData.fRed);
		CheckDlgButton(hDlg, ID_GREEN, shapeData.fGreen);
		CheckDlgButton(hDlg, ID_BLUE, shapeData.fBlue);

		shapeDataNew = shapeData;
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_RECTANGLE:
		case ID_RHOMB:
		case ID_ELLIPSE:
			shapeDataNew.id_shape = LOWORD(wParam);
			return TRUE;

		case ID_RED:
			shapeDataNew.fRed = ~shapeDataNew.fRed;
			return TRUE;
		case ID_GREEN:
			shapeDataNew.fGreen = ~shapeDataNew.fGreen;
			return TRUE;
		case ID_BLUE:
			shapeDataNew.fBlue = ~shapeDataNew.fBlue;
			return TRUE;

		case ID_DARK:
		case ID_MEDIUM:
		case ID_LIGHT:
			shapeDataNew.id_bright = LOWORD(wParam);
			return TRUE;

		case IDOK:
			shapeData = shapeDataNew;			
			EndDialog(hDlg, TRUE);
			return TRUE;

		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			return TRUE;
		}
		break;
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
