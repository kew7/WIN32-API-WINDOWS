//////////////////////////////////////////////////////////////////////
// ToolTip.cpp
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

#define ID_TOOLBAR       201
#define IDC_TB_COMBOBOX  202
#define NUM_BUTTONS         13
#define SEPARATOR_WIDTH     10
#define COMBO_SPACE_WIDTH   50
#define COMBO_SPACE_HEIGHT 100

HWND hwndToolBar;
HWND hwndCombo;

HWND InitToolBar(HWND hWnd);
void UpdateToolBar(ShapeData& sd);

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//==================================================================== 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	KWndEx mainWnd("ToolTip", hInstance, nCmdShow, WndProc,
		MAKEINTRESOURCE(IDR_MENU1), 100, 100, 400, 300);	

	// Добавление окна подсказки
	AddTooltip (mainWnd.GetHWnd(),
		"Это клиентская область главного окна приложения");

	while (GetMessage(&msg, NULL, 0, 0))  {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
//==================================================================== 
HWND InitToolBar(HWND hWnd) {

	HWND hToolBar;
	int btnID[NUM_BUTTONS] = { ID_SEP, ID_SEP, ID_RECTANGLE, ID_RHOMB,
		ID_ELLIPSE, ID_SEP, ID_RED, ID_GREEN, ID_BLUE, ID_SEP,
		ID_DARK, ID_MEDIUM, ID_LIGHT };

	int btnStyle[NUM_BUTTONS] = { TBSTYLE_SEP, TBSTYLE_SEP,
		TBSTYLE_BUTTON, TBSTYLE_BUTTON, TBSTYLE_BUTTON, TBSTYLE_SEP,
		TBSTYLE_CHECK, TBSTYLE_CHECK, TBSTYLE_CHECK, TBSTYLE_SEP,
		TBSTYLE_CHECKGROUP, TBSTYLE_CHECKGROUP, TBSTYLE_CHECKGROUP };
	
	TBBUTTON tbb[NUM_BUTTONS];
	memset(tbb, 0, sizeof(tbb));

	for (int i = 0; i < NUM_BUTTONS; ++i) {
		if (!i)  tbb[i].iBitmap = COMBO_SPACE_WIDTH;
		else if (btnID[i] == ID_SEP)
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

	// Определение позиции и размеров для элемента управления Combo box
	int x, y, cx, cy;
	RECT rcItem;
	SendMessage(hToolBar, TB_GETITEMRECT, 0, (LPARAM)&rcItem);
	x = rcItem.left + 2;     y = rcItem.top;
	cx = COMBO_SPACE_WIDTH;  cy = COMBO_SPACE_HEIGHT;

	// Создание элемента управления Combo box
	hwndCombo = CreateWindow("combobox", NULL, WS_CHILD | WS_VISIBLE |
		CBS_DROPDOWN, x, y, cx, cy, hToolBar, (HMENU)IDC_TB_COMBOBOX,
		GetModuleHandle(NULL), 0);

	// Инициализация списка для hwndCombo
	SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM)"  1  ");
	SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM)"  2  ");
	SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM)"  5  ");
	SendMessage(hwndCombo, CB_ADDSTRING, 0, (LPARAM)" 10  ");
	SendMessage(hwndCombo, CB_SETCURSEL, 0, 0);

	// Добавление окон подсказки
	AddTooltip (hwndCombo, MAKEINTRESOURCE(IDS_TB_COMBOBOX));
	HWND hwndEdit = GetWindow(hwndCombo, GW_CHILD); 
	AddTooltip (hwndEdit, MAKEINTRESOURCE(IDS_EDIT_IN_COMBO));

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

	static HPEN hPen, hOldPen;
	static int indPen;
	int penWidth[] = { 1, 2, 5, 10 };

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
		break;

	// Настройка размеров панели инструментов
	case WM_SIZE:
		SendMessage(hwndToolBar, TB_AUTOSIZE, 0, 0);
		break;

	// Обработка запроса на вывод подсказки
	case WM_NOTIFY:

TRACE("uMsg = %04X, wParam = %08X, lParam = %08X\n",
	  uMsg, wParam, lParam);

		lpTTT = (LPTOOLTIPTEXT)lParam;
		if (lpTTT->hdr.code == TTN_NEEDTEXT) {
			lpTTT->hinst = GetModuleHandle(NULL);
			lpTTT->lpszText = MAKEINTRESOURCE(lpTTT->hdr.idFrom);
		}
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
				"ToolTip\nVersion 1.0\nCopyright: Finesoft Corporation, 2005.",
				"About ToolTip", MB_OK);
			break;

		// Обработка уведомительного сообщения от комбинированного списка
		case IDC_TB_COMBOBOX: 
			switch (HIWORD(wParam)) { 
			case CBN_SELCHANGE: 
				// Выбираем текущий индекс пера
				indPen = SendMessage(hwndCombo, CB_GETCURSEL, 0, 0);				
			} 
			break; 
		default:
			break;
		}

		UpdateToolBar(shapeData);
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		brightness = intensity[shapeData.id_bright - ID_DARK];

		if (bShow) {
			
			hPen = CreatePen(PS_SOLID, penWidth[indPen], RGB(0,0,0));
			hOldPen = (HPEN)SelectObject(hDC, hPen);

			hBrush = CreateSolidBrush(RGB(
				shapeData.fRed? brightness : 0,
				shapeData.fGreen? brightness : 0,
				shapeData.fBlue? brightness : 0));
			hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);

			// Определение центра фигуры (x0, y0)
			GetClientRect(hWnd, &rect);
			// Учитываем уменьшение клиентской области из-за
			// размещения панели инструментов
			GetWindowRect(hwndToolBar, &rcTB);
			tbHeight = rcTB.bottom - rcTB.top;
			x0 = rect.right / 2;
			y0 = tbHeight + (rect.bottom - tbHeight) / 2;

			// Координаты прямоугольника и эллипса
			if (shapeSize == MIN) {
				x1 = x0 - W/2;    y1 = y0 - H/2;
				x2 = x0 + W/2;    y2 = y0 + H/2;
			}
			else {
				x1 = 0;           y1 = tbHeight;
				x2 = rect.right;  y2 = rect.bottom;
			}
			// Координаты ромба
			pt[0].x = (x1 + x2) / 2;  pt[0].y = y1;
			pt[1].x = x2;             pt[1].y = (y1 + y2) / 2;
			pt[2].x = (x1 + x2) / 2;  pt[2].y = y2;
			pt[3].x = x1;             pt[3].y =  (y1 + y2) / 2;

			switch (shapeData.id_shape) {
			case ID_RECTANGLE:
				Rectangle(hDC, x1, y1, x2, y2);  break;
			case ID_RHOMB:
				Polygon(hDC, pt, 4);             break;
			case ID_ELLIPSE:
				Ellipse(hDC,  x1, y1, x2, y2);   break;
			}

			DeleteObject(SelectObject(hDC, hOldBrush));
			DeleteObject(SelectObject(hDC, hOldPen));
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
