//////////////////////////////////////////////////////////////////////
// DlgDemo2.cpp
#include <windows.h>
#include <stdio.h>
#include "KWnd.h"
#include "resource.h"

#define W  200    // ������ ������
#define H  140    // ������ ������
enum ShapeSize { MAX, MIN };

typedef struct {
	int id_shape;   // ������������� ������
	BOOL fRed;      // ���������� �������� �����
	BOOL fGreen;    // ���������� �������� �����
	BOOL fBlue;     // ���������� ������ �����
	int id_bright;  // ������������� ������� �����
} ShapeData;

BOOL CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//==================================================================== 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	KWnd mainWnd("DlgDemo2", hInstance, nCmdShow, WndProc,
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
	static ShapeData shapeData;

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
		CheckMenuRadioItem(GetSubMenu(hMenu, 2), ID_RECTANGLE,
			ID_ELLIPSE, ID_RECTANGLE, MF_BYCOMMAND);
		CheckMenuRadioItem(GetSubMenu(hMenu, 2), ID_DARK,
			ID_LIGHT, ID_DARK, MF_BYCOMMAND);

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

		case ID_RECTANGLE:
			shapeData.id_shape = ID_RECTANGLE;
			CheckMenuRadioItem(GetSubMenu(hMenu, 2), ID_RECTANGLE,
				ID_ELLIPSE, LOWORD(wParam), MF_BYCOMMAND);
			break;
		case ID_RHOMB:
			shapeData.id_shape = ID_RHOMB;
			CheckMenuRadioItem(GetSubMenu(hMenu, 2), ID_RECTANGLE,
				ID_ELLIPSE, LOWORD(wParam), MF_BYCOMMAND);
			break;
		case ID_ELLIPSE:
			shapeData.id_shape = ID_ELLIPSE;
			CheckMenuRadioItem(GetSubMenu(hMenu, 2), ID_RECTANGLE,
				ID_ELLIPSE, LOWORD(wParam), MF_BYCOMMAND);
			break;

		case ID_RED:
			shapeData.fRed = ~shapeData.fRed;
			CheckMenuItem(GetSubMenu(hMenu, 2), ID_RED,
				MF_BYCOMMAND | shapeData.fRed? MF_CHECKED : MF_UNCHECKED);
			break;
		case ID_GREEN:
			shapeData.fGreen = ~shapeData.fGreen;
			CheckMenuItem(GetSubMenu(hMenu, 2), ID_GREEN,
				MF_BYCOMMAND | shapeData.fGreen? MF_CHECKED : MF_UNCHECKED);
			break;
		case ID_BLUE:
			shapeData.fBlue = ~shapeData.fBlue;
			CheckMenuItem(GetSubMenu(hMenu, 2), ID_BLUE,
				MF_BYCOMMAND | shapeData.fBlue? MF_CHECKED : MF_UNCHECKED);
			break;

		case ID_DARK:
			shapeData.id_bright = ID_DARK;
			CheckMenuRadioItem(GetSubMenu(hMenu, 2), ID_DARK,
				ID_LIGHT, LOWORD(wParam), MF_BYCOMMAND);
			break;
		case ID_MEDIUM:
			shapeData.id_bright = ID_MEDIUM;
			CheckMenuRadioItem(GetSubMenu(hMenu, 2), ID_DARK,
				ID_LIGHT, LOWORD(wParam), MF_BYCOMMAND);
			break;
		case ID_LIGHT:
			shapeData.id_bright = ID_LIGHT;
			CheckMenuRadioItem(GetSubMenu(hMenu, 2), ID_DARK,
				ID_LIGHT, LOWORD(wParam), MF_BYCOMMAND);
			break;

		case IDM_RESIZE:
			shapeSize = (shapeSize == MIN)? MAX : MIN;
			ModifyMenu(hMenu, IDM_RESIZE, MF_BYCOMMAND, IDM_RESIZE,
				itemResizeName[shapeSize]);
			DrawMenuBar(hWnd);
			break;

		case IDM_ABOUT:

			// ����� �������
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUT), hWnd, AboutDlgProc);  
			
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
	HINSTANCE hInst;
	static HWND hStatic1;
	static LOGFONT lf;  // ����������� static ��� ������������� ����� ������!
	static HFONT hFont1, hFont2;

	static HBITMAP hPict0;   // ����������� ��������� ������
	static HBITMAP hPict1;   // ����������� �������� ������
	static HBITMAP hPict2;   // ����������� ������� ������
	static HBRUSH hBrush0;   // ��� ��������� ������
	static HBRUSH hBrush1;   // ��� �������� ������
	static HBRUSH hBrush2;   // ��� ������� ������
	static HBRUSH hCurBrush; // ������� ���		
	static HDC hMemDC;       // �������� ���������� � ������
	static BITMAP bm;        // ��������� ������

	LPDRAWITEMSTRUCT pdis;
	static HWND hBtnOk, hBtnHelp;
	static RECT rcBtnHelp, rect;
	int x0, y0 = 0;

	switch (uMsg) {
	case WM_INITDIALOG:
		hInst = GetModuleHandle(NULL);
		hStatic1 = GetDlgItem(hDlg, IDC_STATIC_1);
		hBtnOk = GetDlgItem(hDlg, IDOK);
		hBtnHelp = GetDlgItem(hDlg, ID_ABOUT_HELP);
		GetWindowRect(hBtnHelp, &rcBtnHelp);

		// �������� ����������� ������ Verdana
		lf.lfHeight = 28;
		lstrcpy( (LPSTR)&lf.lfFaceName, "Verdana" );
		hFont1 = CreateFontIndirect(&lf);
		lf.lfHeight = 20;
		hFont2 = CreateFontIndirect(&lf);
		// ����������� ������ ��� �������� hStatic1
		SendMessage(hStatic1, WM_SETFONT, (WPARAM)hFont1, TRUE );  

		// ������������� ��������� ����������� ������ IDOK
		hPict0 = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_PICT_0));
		hPict1 = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_PICT_1));
		hPict2 = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_PICT_2));
		hBrush0 = CreateSolidBrush(RGB(214, 214, 214));
		hBrush1 = CreateSolidBrush(RGB(149, 242, 242));
		hBrush2 = CreateSolidBrush(RGB(83, 52, 254));

		GetObject(hPict0, sizeof(bm), (LPSTR)&bm);
		SetFocus(hBtnHelp);
		return FALSE; // ����� ����� ����� ������� �� hBtnHelp

	case WM_CTLCOLORSTATIC:
		// ����������� �������� ��������� �������� hStatic1
		if ((HWND)lParam == hStatic1) {
			SetTextColor((HDC)wParam, RGB(160, 0, 0));
			SetBkMode((HDC)wParam, TRANSPARENT);
			return (DWORD)GetSysColorBrush(COLOR_3DFACE);
		}
		break;

	case WM_DRAWITEM:  // ���������, ������������ ���
					   // ����������� ������ Owner draw
		pdis = (LPDRAWITEMSTRUCT)lParam;
		hMemDC = CreateCompatibleDC(pdis->hDC);

		// ���������, ����� ������� ���������
		switch (pdis->itemAction) {
		case ODA_FOCUS:
			if (pdis->itemState & ODS_FOCUS) {
				SelectObject(hMemDC, hPict1);
				hCurBrush = hBrush1;
			}
			else {
				SelectObject(hMemDC, hPict0);
				hCurBrush = hBrush0;
			}
			break;

		case ODA_SELECT:
			if (pdis->itemState & ODS_SELECTED) {
				SelectObject(hMemDC, hPict2);
				hCurBrush = hBrush2;
			}
			break;

		default:
			SelectObject(hMemDC, hPict0);
			hCurBrush = hBrush0;
		} // end of switch

		// ����� ����������� ������ � ������ ������������ �������
		FillRect(pdis->hDC, &pdis->rcItem, hCurBrush);
		x0 = ((pdis->rcItem.right - pdis->rcItem.left) - bm.bmWidth) / 2;
		// �������� ...
		BitBlt(pdis->hDC, x0, y0, bm.bmWidth, bm.bmHeight,
				hMemDC, 0, 0, SRCCOPY);
		// � ������ �����
		SelectObject(pdis->hDC, hFont2);
		SetBkMode(pdis->hDC, TRANSPARENT);
		SetTextColor(pdis->hDC, RGB(255, 255, 255));
		TextOut(pdis->hDC, 32, 74, "OK", 2);
		SetTextColor(pdis->hDC, RGB(100, 100, 100));
		TextOut(pdis->hDC, 30, 72, "OK", 2);
		// ����� ������ ������
		FrameRect(pdis->hDC, &pdis->rcItem,
			(HBRUSH)GetStockObject(BLACK_BRUSH));
		DeleteDC(hMemDC); // !!! �� �������� ������� hMemDC
		return TRUE;

	case WM_SETCURSOR:  // ������������ ������ ����� ��� ������� ����
		if ((HWND)wParam == hBtnOk)	
			if (GetFocus() != hBtnOk)  SetFocus(hBtnOk);

		if ((HWND)wParam == hBtnHelp) {
			if (GetFocus() != hBtnHelp) {
				SetFocus(hBtnHelp);
				// ������ ���������� ����� ������� DrawFocusRect
				SetRect(&rect, 3, 3, rcBtnHelp.right - rcBtnHelp.left - 3,
					rcBtnHelp.bottom - rcBtnHelp.top - 3);
				DrawFocusRect(GetDC(hBtnHelp), &rect);
			}
		}
		return FALSE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_ABOUT_HELP:
			MessageBox(hDlg, "� ���� ���������� ���� �������� ������ 'Owner draw'",
				"HELP", MB_OK); 
			return TRUE;

		case IDOK:
		case IDCANCEL:
			DeleteObject(hFont1);
			DeleteObject(hFont2);
			EndDialog(hDlg, 0);
			return TRUE;
		}
		break;
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
