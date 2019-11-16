//////////////////////////////////////////////////////////////////////
// KWndPlut.cpp
#include "KWndPlut.h"

KWnd::KWnd(LPCTSTR windowName, HINSTANCE hInst, int cmdShow,
				   LRESULT (WINAPI *pWndProc)(HWND,UINT,WPARAM,LPARAM),
				   LPCTSTR menuName, int x, int y, int width, int height,
				   UINT classStyle, DWORD windowStyle, HWND hParent)
{
	char szClassName[] = "KWndClass";

	wc.cbSize        = sizeof(wc);
	wc.style         = classStyle;
	wc.lpfnWndProc   = pWndProc;
	wc.cbClsExtra	 = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInst;
	wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = menuName;
	wc.lpszClassName = szClassName;
	wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

	// Регистрируем класс окна
	if (!RegisterClassEx(&wc)) {
		char msg[100] = "Cannot register class: ";
		strcat(msg, szClassName);
		MessageBox(NULL, msg, "Error", MB_OK);
		return;
	}
	
	// Создаем окно
	hWnd = CreateWindow(szClassName, windowName, windowStyle,
		x, y, width, height, hParent, (HMENU)NULL, hInst, NULL);       
	
	if (!hWnd) {
		char text[100] = "Cannot create window: ";
		strcat(text, windowName);
		MessageBox(NULL, text, "Error", MB_OK);
		return;
	}

	// Показываем  окно
	ShowWindow(hWnd, cmdShow); 
}

//====================================================================
//  Утилиты общего применения
//====================================================================
// Сдвиг окна верхнего уровня с модификацией размеров
void ShiftWindow(HWND hwnd, int dX, int dY, int dW, int dH) {
	RECT rect;
	
	GetWindowRect(hwnd, &rect);
	int x0 = rect.left + dX;
	int y0 = rect.top + dY;
	int width = rect.right - rect.left + dW;
	int height = rect.bottom - rect.top + dH;
	MoveWindow(hwnd, x0, y0, width, height, TRUE);
}

// Сдвиг дочернего окна с модификацией размеров
void ShiftWindow(HWND hChild, HWND hParent, int dX, int dY, int dW, int dH) {
	RECT rect;
	POINT p0;

	GetWindowRect(hChild, &rect);
	int width = rect.right - rect.left + dW;
	int height = rect.bottom - rect.top + dH;

	p0.x = rect.left + dX;
	p0.y = rect.top + dY;
	ScreenToClient(hParent, &p0);

	MoveWindow(hChild, p0.x, p0.y, width, height, TRUE);
}

// Сдвиг окна элемента управления с модификацией размеров
void ShiftWindow(int ctrlID, HWND hParent, int dX, int dY, int dW, int dH) {
	RECT rect;
	HWND hCtrl = GetDlgItem(hParent, ctrlID);
	POINT p0;

	GetWindowRect(hCtrl, &rect);
	int width = rect.right - rect.left + dW;
	int height = rect.bottom - rect.top + dH;

	p0.x = rect.left + dX;
	p0.y = rect.top + dY;
	ScreenToClient(hParent, &p0);

	ShowWindow(hCtrl, SW_HIDE);
	MoveWindow(hCtrl, p0.x, p0.y, width, height, TRUE);
	ShowWindow(hCtrl, SW_SHOW);
}


