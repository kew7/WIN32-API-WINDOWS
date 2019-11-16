//////////////////////////////////////////////////////////////////////
// StatusBar.cpp
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include "KWndEx.h"
#include "resource.h"

#define W  200    // ширина фигуры
#define H  140    // высота фигуры
enum ShapeSize { MAX, MIN };

typedef struct {
	int id_shape;   // идентификатор фигуры
	BOOL fRed;      // компонента красного цвета
	BOOL fGreen;    // компонента зеленого цвета
	BOOL fBlue;     // компонента синего цвета
	int id_bright;  // идентификатор яркости цвета
} ShapeData;

#define ID_TOOLBAR 201
#define ID_STATUSBAR 202
#define NUM_BUTTONS  11
#define SEPARATOR_WIDTH 10

#define N_PARTS 4

HWND hwndToolBar;
HWND hwndStatusBar;

HWND InitToolBar(HWND hWnd);
void UpdateToolBar(ShapeData& sd);
void UpdateStatusBar(HWND hwnd, ShapeData& sd);

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//==================================================================== 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	KWndEx mainWnd("StatusBar", hInstance, nCmdShow, WndProc,
		MAKEINTRESOURCE(IDR_MENU1), 100, 100, 400, 300);	

	// Инициализация библиотеки "Common Control Library"
	INITCOMMONCONTROLSEX icc;
	icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icc.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&icc);

	while (GetMessage(&msg, NULL, 0, 0))  {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
//==================================================================== 
HWND InitToolBar(HWND hWnd) {

	HWND hToolBar;
	int btnID[NUM_BUTTONS] = { ID_RECTANGLE, ID_RHOMB, ID_ELLIPSE, ID_SEP,
		ID_RED, ID_GREEN, ID_BLUE, ID_SEP, ID_DARK, ID_MEDIUM, ID_LIGHT };

	int btnStyle[NUM_BUTTONS] = { TBSTYLE_BUTTON, TBSTYLE_BUTTON, TBSTYLE_BUTTON,
		TBSTYLE_SEP, TBSTYLE_CHECK, TBSTYLE_CHECK, TBSTYLE_CHECK, TBSTYLE_SEP,
		TBSTYLE_CHECKGROUP, TBSTYLE_CHECKGROUP, TBSTYLE_CHECKGROUP };
	
	TBBUTTON tbb[NUM_BUTTONS];
	memset(tbb, 0, sizeof(tbb));

	for (int i = 0; i < NUM_BUTTONS; ++i) {
		if (btnID[i] == ID_SEP)
			tbb[i].iBitmap = SEPARATOR_WIDTH;
		else  tbb[i].iBitmap = i;

		tbb[i].idCommand = btnID[i];
		tbb[i].fsState = TBSTATE_ENABLED;
		tbb[i].fsStyle = btnStyle[i];
	}

	hToolBar = CreateToolbarEx(hWnd,
		WS_CHILD | WS_VISIBLE | WS_BORDER | TBSTYLE_TOOLTIPS, 
		ID_TOOLBAR, NUM_BUTTONS, GetModuleHandle(NULL), IDR_TOOLBAR1,
		tbb, NUM_BUTTONS, 0, 0, 0, 0, sizeof(TBBUTTON));
	return hToolBar;
}
//==================================================================== 
void UpdateToolBar(ShapeData& sd) {
	SendMessage(hwndToolBar, TB_CHECKBUTTON, ID_RED, sd.fRed);
	SendMessage(hwndToolBar, TB_CHECKBUTTON, ID_GREEN, sd.fGreen);
	SendMessage(hwndToolBar, TB_CHECKBUTTON, ID_BLUE, sd.fBlue);

	SendMessage(hwndToolBar, TB_CHECKBUTTON, ID_DARK,
		(sd.id_bright == ID_DARK));
	SendMessage(hwndToolBar, TB_CHECKBUTTON, ID_MEDIUM,
		(sd.id_bright == ID_MEDIUM));
	SendMessage(hwndToolBar, TB_CHECKBUTTON, ID_LIGHT,
		(sd.id_bright == ID_LIGHT));
}
//==================================================================== 
void UpdateStatusBar(HWND hwnd, ShapeData& sd) {
	
	int paneWidth;
	int aWidths[N_PARTS];
	char text[100];
	RECT rect;
	int brightness;
	int intensity[3] = { 85, 170, 255 };

	GetClientRect(hwnd, &rect);

	paneWidth = rect.right / N_PARTS;
	aWidths [0] = paneWidth;
	aWidths [1] = paneWidth * 2;
	aWidths [2] = paneWidth * 3;
	aWidths [3] = -1;
	SendMessage(hwndStatusBar, SB_SETPARTS, N_PARTS, (LPARAM)aWidths);

	LoadString(GetModuleHandle(NULL), sd.id_shape, text, 100);
	SendMessage(hwndStatusBar, SB_SETTEXT, 0, (LPARAM)text);

	brightness = intensity[sd.id_bright - ID_DARK];

	int red = sd.fRed? brightness : 0;
	sprintf(text, "Red = %d", red); 
	SendMessage(hwndStatusBar, SB_SETTEXT, 1, (LPARAM)text);

	int green = sd.fGreen? brightness : 0;
	sprintf(text, "Green = %d", green); 
	SendMessage(hwndStatusBar, SB_SETTEXT, 2, (LPARAM)text);

	int blue = sd.fBlue? brightness : 0;
	sprintf(text, "Blue = %d", blue); 
	SendMessage(hwndStatusBar, SB_SETTEXT, 3, (LPARAM)text);
}
//==================================================================== 
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hDC;
	PAINTSTRUCT ps;
	RECT rect;
	static HMENU hMenu;  // дескриптор главного меню
	int x0, y0, x1, y1, x2, y2;
	POINT pt[4];
	static ShapeSize shapeSize = MIN;
	static BOOL bShow = TRUE;
	static HBRUSH hBrush, hOldBrush;
	char* itemResizeName[2] = { "Decrease!", "Increase!"};
	int intensity[3] = { 85, 170, 255 }; // интенсивность RGB-компонент цвета
	int brightness;
	static ShapeData shapeData;

	RECT rcTB;     // позиция и размеры окна hwndToolBar
	int tbHeight;  // высота окна hwndToolBar
	LPTOOLTIPTEXT lpTTT;

	RECT rcSB;     // позиция и размеры окна hwndStatusBar
	int sbHeight;  // высота окна hwndStatusBar
	HMENU hClickMenu;
	int itemID;
	static UINT submenuID;
	char text[200];

	switch (uMsg)
	{
	case WM_CREATE:
		hMenu = GetMenu(hWnd);
		// IDM_OPEN - пункт меню, применяемый по умолчанию
		SetMenuDefaultItem(GetSubMenu(hMenu, 0), IDM_OPEN, FALSE);

		// Исходные отметки пунктов меню 
		CheckMenuRadioItem(GetSubMenu(hMenu, 1), IDM_SHOW_SHAPE,
			IDM_HIDE_SHAPE, IDM_SHOW_SHAPE, MF_BYCOMMAND);
		CheckMenuRadioItem(GetSubMenu(hMenu, 2), ID_RECTANGLE,
			ID_ELLIPSE, ID_RECTANGLE, MF_BYCOMMAND);
		CheckMenuRadioItem(GetSubMenu(hMenu, 2), ID_DARK,
			ID_LIGHT, ID_DARK, MF_BYCOMMAND);
		shapeData.id_shape = ID_RECTANGLE;
		shapeData.id_bright = ID_DARK;

		// Создание панели инструментов
		hwndToolBar = InitToolBar(hWnd);
		SendMessage(hwndToolBar, TB_AUTOSIZE, 0, 0);

		// Создание строки состояния
		hwndStatusBar = CreateStatusWindow(WS_CHILD | WS_VISIBLE, "", hWnd, ID_STATUSBAR);

		break;

	case WM_SIZE:
		SendMessage(hwndToolBar, TB_AUTOSIZE, 0, 0);

		UpdateStatusBar(hWnd, shapeData);
		SendMessage (hwndStatusBar, WM_SIZE, wParam, lParam);
		break;

	case WM_NOTIFY:
		lpTTT = (LPTOOLTIPTEXT)lParam;
		if (lpTTT->hdr.code == TTN_NEEDTEXT) {
			lpTTT->hinst = GetModuleHandle(NULL);
			lpTTT->lpszText = MAKEINTRESOURCE(lpTTT->hdr.idFrom);
		}
		break;


	case WM_MENUSELECT:
		itemID = LOWORD(wParam);
		hClickMenu = (HMENU)lParam;

		if (HIWORD(wParam) & MF_POPUP) {
			if (hClickMenu == hMenu)
				submenuID = itemID;
			else
				itemID += 100 * (submenuID + 1);
		}
		else if (!itemID)
			itemID = -1;

		SendMessage(hwndStatusBar, SB_SIMPLE, TRUE, 0);
		LoadString(GetModuleHandle(NULL), itemID, text, 200);
		SendMessage(hwndStatusBar, SB_SETTEXT, 255, (LPARAM)text);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_OPEN:
			MessageBox(hWnd, "Выбран пункт 'Open'", "Меню File", MB_OK);
			break;
		case IDM_CLOSE:
			MessageBox(hWnd, "Выбран пункт 'Close'", "Меню File", MB_OK);
			break;		
		case IDM_SAVE:
			MessageBox(hWnd, "Выбран пункт 'Save'", "Меню File", MB_OK);
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
			MessageBox(hWnd,
				"StatusBar\nVersion 1.0\nCopyright: Finesoft Corporation, 2005.",
				"About StatusBar", MB_OK);
			break;

		default:
			break;
		}

		UpdateToolBar(shapeData);
		SendMessage(hwndStatusBar, SB_SIMPLE, FALSE, 0);
		UpdateStatusBar(hWnd, shapeData);

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

			// Определение позиции и размеров фигуры с учетом
			// уменьшения клиентской области из-за размещения
			// панели инструментов и строки состояния
			GetClientRect(hWnd, &rect);
			GetWindowRect(hwndToolBar, &rcTB);
			tbHeight = rcTB.bottom - rcTB.top;
			GetWindowRect(hwndStatusBar, &rcSB);
			sbHeight = rcSB.bottom - rcSB.top;

			x0 = rect.right / 2;
			y0 = tbHeight + (rect.bottom - tbHeight - sbHeight) / 2;

			// Координаты прямоугольника и эллипса
			if (shapeSize == MIN) {
				x1 = x0 - W/2;    y1 = y0 - H/2;
				x2 = x0 + W/2;    y2 = y0 + H/2;
			}
			else {
				x1 = 0;           y1 = tbHeight;
				x2 = rect.right;  y2 = rect.bottom - sbHeight;
			}

			// Координаты ромба
			pt[0].x = (x1 + x2) / 2;  pt[0].y = y1;
			pt[1].x = x2;             pt[1].y = (y1 + y2) / 2;
			pt[2].x = (x1 + x2) / 2;  pt[2].y = y2;
			pt[3].x = x1;             pt[3].y =  (y1 + y2) / 2;

			switch (shapeData.id_shape) {
			case ID_RECTANGLE:
				Rectangle(hDC, x1, y1, x2, y2);	 break;
			case ID_RHOMB:
				Polygon(hDC, pt, 4);             break;
			case ID_ELLIPSE:
				Ellipse(hDC,  x1, y1, x2, y2);   break;
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
//////////////////////////////////////////////////////////////////////
