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
//  ������� ������ ����������
//==========================================================
// ����� ���� �������� ������ � ������������ ��������
void ShiftWindow(HWND hwnd, int dX = 0, int dY = 0,
				 int dW = 0, int dH = 0);
// ����� ��������� ���� � ������������ ��������
void ShiftWindow(HWND hChild, HWND hParent, int dX = 0,
				 int dY = 0, int dW = 0, int dH = 0);
// ����� ���� �������� ���������� � ������������ ��������
void ShiftWindow(int ctrlID, HWND hParent, int dX = 0,
				 int dY = 0, int dW = 0, int dH = 0);
