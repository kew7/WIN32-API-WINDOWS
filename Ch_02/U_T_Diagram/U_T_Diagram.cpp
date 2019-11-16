//////////////////////////////////////////////////////////////////////
// U_T_Diagram.cpp
#include <windows.h>
#include <math.h>
#include "KWnd.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void DrawDiagram(HWND, HDC);

#define Pi 3.14159265

//////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	KWnd mainWnd("Временная диаграмма напряжения переменного электрического тока", hInstance, nCmdShow, WndProc);	

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

	switch (uMsg)
	{
	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);

		DrawDiagram(hWnd, hDC);

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
void DrawDiagram(HWND hwnd, HDC hdc)
{
	RECT rect;
	GetClientRect(hwnd, &rect);
	const int xVE = rect.right - rect.left;
	const int yVE = rect.bottom - rect.top;
	const int xWE = xVE; 
	const int yWE = yVE;

	double nPixPerVolt = yVE / 1000.0;
	double nPixPerMs = xVE / 60.0;  // n Pixel Per Millisec

	SetMapMode (hdc, MM_ISOTROPIC);
	SetWindowExtEx (hdc, xWE, yWE, NULL);
	SetViewportExtEx (hdc, xVE, -yVE, NULL);
	SetViewportOrgEx (hdc, 10*nPixPerMs, yVE/2, NULL);

	int tMin = 0;     // ms
	int tMax = 40;    // ms
	int uMin = -400;  // V
	int uMax = 400;   // V

	int tGridStep = 5;
	int uGridStep = 100;
	int x, y;

	char* xMark[] = { "0", "5", "10", "15", "20", "25", "30", "35", "40"};
	char* yMark[] = { "-400","-300","-200","-100","0","100","200","300","400"};


	// Сетка
	HPEN hPen0 = CreatePen(PS_SOLID, 1, RGB(0, 160, 0));
	HPEN hOldPen = (HPEN)SelectObject(hdc, hPen0);
	int u = uMin;
	int xMin = tMin * nPixPerMs;
	int xMax = tMax * nPixPerMs;
	for (int i = 0; i < 9; ++i) {
		y = u * nPixPerVolt;
		MoveToEx(hdc, xMin, y, NULL);
		LineTo(hdc, xMax, y);
		TextOut(hdc, xMin-40, y+8, yMark[i], strlen(yMark[i]));
		u += uGridStep;
	}

	int t = tMin;
	int yMin = uMin * nPixPerVolt;
	int yMax = uMax * nPixPerVolt;
	for (int i = 0; i < 9; ++i) {
		x = t * nPixPerMs;
		MoveToEx(hdc, x, yMin, NULL);
		LineTo(hdc, x, yMax);
		TextOut(hdc, x-6, yMin-10, xMark[i], strlen(xMark[i]));
		t += tGridStep;
	}

	// Ось "x"
	HPEN hPen1 = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
	SelectObject(hdc, hPen1);
   	MoveToEx(hdc, 0, 0, NULL);  LineTo(hdc, xMax, 0);

	static LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfPitchAndFamily = FIXED_PITCH | FF_MODERN;
	lf.lfItalic = TRUE;
	lf.lfWeight = FW_BOLD;
	lf.lfHeight = 20;
	lf.lfCharSet = DEFAULT_CHARSET;
	lstrcpy( (LPSTR)&lf.lfFaceName, "Arial" );

	HFONT hFont0 = CreateFontIndirect(&lf);
	HFONT hOldFont = (HFONT)SelectObject(hdc, hFont0);
	SetTextColor(hdc, RGB(0, 0, 200));

	TextOut(hdc, xMax+10, 10, "t (ms)", 6);
	// Ось "y"
	MoveToEx(hdc, 0, yMin, NULL);  LineTo(hdc, 0, yMax);
	TextOut(hdc, -10, yMax+30, "u (V)", 5);

	// График
	HPEN hPen2 = CreatePen(PS_SOLID, 5, RGB(200, 0, 100));
	SelectObject(hdc, hPen2);
	int tStep = 1;
	double radianPerMs = 2 * Pi / 20;
	const double uAmplit = 311.17;       // volt
	t = tMin;
	MoveToEx(hdc, 0, 0, NULL);
	while (t <= tMax) {
		u = uAmplit * sin( t * radianPerMs);
		LineTo(hdc, t * nPixPerMs, u * nPixPerVolt);
		t += tStep;
	}

	// Заголовок
	char* title = "Диаграмма напряжения переменного электр. тока"; 
	lf.lfItalic = FALSE;
	lf.lfWeight = FW_BOLD;
	lf.lfHeight = 30;
	HFONT hFont1 = CreateFontIndirect(&lf);
	SelectObject(hdc, hFont1);
	SetTextColor(hdc, RGB(0, 200, 0));
	TextOut(hdc, 0, yMax + 70, title, strlen(title));

	SelectObject(hdc, hOldPen);
	SelectObject(hdc, hOldFont);
}