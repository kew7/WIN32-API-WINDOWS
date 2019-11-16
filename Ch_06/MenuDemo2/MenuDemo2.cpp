//////////////////////////////////////////////////////////////////////
// MenuDemo2.cpp
#include <windows.h>
#include <stdio.h>
#include "KWnd.h"
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

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//==================================================================== 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	KWnd mainWnd("MenuDemo2", hInstance, nCmdShow, WndProc,
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
	RECT rect;
	static HMENU hMenu;  // дескриптор главного меню
	static HMENU hMenuShape;  // дескриптор 1-го контекстного меню
	static HMENU hMenuColor;  // дескриптор 2-го контекстного меню
	POINT point, ptInClient;
	int x0, y0, x1, y1, x2, y2;
	POINT pt[4];
	static ShapeSize shapeSize = MIN;
	static BOOL bShow = TRUE;
	static HBRUSH hBrush, hOldBrush;
	char* itemResizeName[2] = { "Decrease!", "Increase!"};
	int intensity[3] = { 85, 170, 255 }; // интенсивность RGB-компонент цвета
	int brightness;
	static ShapeData shapeData;

	switch (uMsg)
	{
	case WM_CREATE:
		hMenu = GetMenu(hWnd);
		hMenuShape = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MENU_SHAPE));
		hMenuShape = GetSubMenu(hMenuShape, 0);

		hMenuColor = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MENU_COLOR));
		hMenuColor = GetSubMenu(hMenuColor, 0);

		// IDM_OPEN - пункт меню, применяемый по умолчанию
		SetMenuDefaultItem(GetSubMenu(hMenu, 0), IDM_OPEN, FALSE);

		// Исходные отметки пунктов меню 
		CheckMenuRadioItem(GetSubMenu(hMenu, 1), IDM_SHOW_SHAPE,
			IDM_HIDE_SHAPE, IDM_SHOW_SHAPE, MF_BYCOMMAND);
		CheckMenuRadioItem(hMenuShape, ID_RECTANGLE,
			ID_ELLIPSE, ID_RECTANGLE, MF_BYCOMMAND);
		CheckMenuRadioItem(hMenuColor, ID_DARK,
			ID_LIGHT, ID_DARK, MF_BYCOMMAND);

		shapeData.id_shape = ID_RECTANGLE;
		shapeData.id_bright = ID_DARK;

		break;

	case WM_CONTEXTMENU:
		point.x	= LOWORD(lParam);
		point.y	= HIWORD(lParam);
		ptInClient = point;
		ScreenToClient(hWnd, &ptInClient);
		GetClientRect(hWnd, &rect);
		
		if (ptInClient.x < rect.right / 2)
			TrackPopupMenuEx(hMenuShape, 0, point.x, point.y, hWnd, NULL);
		else
			TrackPopupMenuEx(hMenuColor, 0, point.x, point.y, hWnd, NULL);
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
			CheckMenuRadioItem(hMenuShape, ID_RECTANGLE,
				ID_ELLIPSE, LOWORD(wParam), MF_BYCOMMAND);
			break;
		case ID_RHOMB:
			shapeData.id_shape = ID_RHOMB;
			CheckMenuRadioItem(hMenuShape, ID_RECTANGLE,
				ID_ELLIPSE, LOWORD(wParam), MF_BYCOMMAND);
			break;
		case ID_ELLIPSE:
			shapeData.id_shape = ID_ELLIPSE;
			CheckMenuRadioItem(hMenuShape, ID_RECTANGLE,
				ID_ELLIPSE, LOWORD(wParam), MF_BYCOMMAND);
			break;

		case ID_RED:
			shapeData.fRed = ~shapeData.fRed;
			CheckMenuItem(hMenuColor, ID_RED,
				MF_BYCOMMAND | shapeData.fRed? MF_CHECKED : MF_UNCHECKED);
			break;
		case ID_GREEN:
			shapeData.fGreen = ~shapeData.fGreen;
			CheckMenuItem(hMenuColor, ID_GREEN,
				MF_BYCOMMAND | shapeData.fGreen? MF_CHECKED : MF_UNCHECKED);
			break;
		case ID_BLUE:
			shapeData.fBlue = ~shapeData.fBlue;
			CheckMenuItem(hMenuColor, ID_BLUE,
				MF_BYCOMMAND | shapeData.fBlue? MF_CHECKED : MF_UNCHECKED);
			break;

		case ID_DARK:
			shapeData.id_bright = ID_DARK;
			CheckMenuRadioItem(hMenuColor, ID_DARK,
				ID_LIGHT, LOWORD(wParam), MF_BYCOMMAND);
			break;
		case ID_MEDIUM:
			shapeData.id_bright = ID_MEDIUM;
			CheckMenuRadioItem(hMenuColor, ID_DARK,
				ID_LIGHT, LOWORD(wParam), MF_BYCOMMAND);
			break;
		case ID_LIGHT:
			shapeData.id_bright = ID_LIGHT;
			CheckMenuRadioItem(hMenuColor, ID_DARK,
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
				"MenuDemo2\nVersion 1.0\nCopyright: Finesoft Corporation, 2005.",
				"About MenuDemo2", MB_OK);
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

			// Определение центра фигуры (x0, y0)
			GetClientRect(hWnd, &rect);
			x0 = rect.right / 2;
			y0 = rect.bottom / 2;
			// Координаты прямоугольника и эллипса
			if (shapeSize == MIN) {
				x1 = x0 - W/2;    y1 = y0 - H/2;
				x2 = x0 + W/2;    y2 = y0 + H/2;
			}
			else {
				x1 = y1 = 0;
				x2 = rect.right;  y2 = rect.bottom;
			}
			// Координаты ромба
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
//////////////////////////////////////////////////////////////////////
