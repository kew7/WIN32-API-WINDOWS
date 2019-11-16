////////////////////////////////////////////////////////////
// KWndPlut.h
#include <windows.h>

class KWnd {
public:	
	KWnd(LPCTSTR windowName, HINSTANCE hInst, int cmdShow,
		LRESULT (WINAPI *pWndProc)(HWND,UINT,WPARAM,LPARAM),
		LPCTSTR menuName = NULL,
		int x = CW_USEDEFAULT, int y = 0,
		int width = CW_USEDEFAULT, int height = 0,
		UINT classStyle =  CS_HREDRAW | CS_VREDRAW,
		DWORD windowStyle = WS_OVERLAPPEDWINDOW,
		HWND hParent = NULL);

	HWND GetHWnd() { return hWnd; }
protected:
	HWND hWnd;  
	WNDCLASSEX wc;
};

//==========================================================
//  Утилиты общего применения
//==========================================================
// Сдвиг окна верхнего уровня с модификацией размеров
void ShiftWindow(HWND hwnd, int dX = 0, int dY = 0,
				 int dW = 0, int dH = 0);
// Сдвиг дочернего окна с модификацией размеров
void ShiftWindow(HWND hChild, HWND hParent, int dX = 0,
				 int dY = 0, int dW = 0, int dH = 0);
// Сдвиг окна элемента управления с модификацией размеров
void ShiftWindow(int ctrlID, HWND hParent, int dX = 0,
				 int dY = 0, int dW = 0, int dH = 0);
