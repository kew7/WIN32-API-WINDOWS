//////////////////////////////////////////////////////////////////////
// TrackBar.cpp
#include <windows.h>
#include <stdio.h>
#include <commctrl.h>

#include "KWndEx.h"
#include "resource.h"

enum UserMsg { UM_CHANGE = WM_USER+1 };

HWND hModelessDlg;
RECT rcWork; // прямоугольник рабочей области
int rgb[3];  // интенсивность для R-, G-, B- компонент цвета

BOOL CALLBACK ModelessDlgProc(HWND, UINT, WPARAM, LPARAM);
void AdjustDlgPlace(HWND, HWND);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//==================================================================== 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;

	KWndEx mainWnd("TrackBar", hInstance, nCmdShow, WndProc,
		NULL, 100, 100, 400, 270);	

	while (GetMessage(&msg, NULL, 0, 0))  {
		if (!IsDialogMessage(hModelessDlg, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return msg.wParam;
}

//==================================================================== 
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {

	case WM_CREATE:
		hModelessDlg = CreateDialog(GetModuleHandle(NULL),
			MAKEINTRESOURCE(IDD_MODELESS), hWnd, ModelessDlgProc);
		AdjustDlgPlace(hWnd, hModelessDlg);
		break;

	case WM_SIZE:
		AdjustDlgPlace(hWnd, hModelessDlg);
		break;

	case UM_CHANGE:
		// Изменение цвета фона основного окна
		SetClassLong(hWnd, GCL_HBRBACKGROUND, (LONG)CreateSolidBrush(
			RGB(rgb[0], rgb[1], rgb[2])));
		InvalidateRect(hWnd, &rcWork, TRUE);
		break;

	case WM_DESTROY:
		DestroyWindow(hModelessDlg);
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

//==================================================================== 
BOOL CALLBACK ModelessDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hSliderRed;
	static HWND hSliderGreen;
	static HWND hSliderBlue;
	static HWND hStaticRed;
	static HWND hStaticGreen;
	static HWND hStaticBlue;
	char text[100];

	switch (uMsg) {
	case WM_INITDIALOG:

		// Определяем дескрипторы элементов управления Slider
		hSliderRed = GetDlgItem(hDlg, IDC_SLIDER_RED);
		hSliderGreen = GetDlgItem(hDlg, IDC_SLIDER_GREEN);
		hSliderBlue = GetDlgItem(hDlg, IDC_SLIDER_BLUE);
		hStaticRed = GetDlgItem(hDlg, IDC_STATIC_RED);
		hStaticGreen = GetDlgItem(hDlg, IDC_STATIC_GREEN);
		hStaticBlue = GetDlgItem(hDlg, IDC_STATIC_BLUE);

		// Инициализация элементов управления Slider
		SendMessage(hSliderRed, TBM_SETRANGE, TRUE, MAKELONG(0, 255));
		SendMessage(hSliderRed, TBM_SETTICFREQ, 32, 0);
		SendMessage(hSliderGreen, TBM_SETRANGE, TRUE, MAKELONG(0, 255));
		SendMessage(hSliderGreen, TBM_SETTICFREQ, 32, 0);
		SendMessage(hSliderBlue, TBM_SETRANGE, TRUE, MAKELONG(0, 255));
		SendMessage(hSliderBlue, TBM_SETTICFREQ, 32, 0);

		// Вывод фона окна для начального состояния регуляторов
		SendMessage(hDlg, WM_HSCROLL, 0, 0);
		return TRUE;

	case WM_HSCROLL:
		
		// Получаем текущие позиции элементов управления Slider		
		rgb[0] = SendMessage(hSliderRed, TBM_GETPOS, 0, 0);
		sprintf(text, "Red = %d", rgb[0]);
		SetWindowText(hStaticRed, text);

		rgb[1] = SendMessage(hSliderGreen, TBM_GETPOS, 0, 0);
		sprintf(text, "Green = %d", rgb[1]);
		SetWindowText(hStaticGreen, text);
		
		rgb[2] = SendMessage(hSliderBlue, TBM_GETPOS, 0, 0);
		sprintf(text, "Blue = %d", rgb[2]);
		SetWindowText(hStaticBlue, text);

		// Сообщение родительскому окну об изменении цвета фона
		SendMessage(GetParent(hDlg), UM_CHANGE, 0, 0);
		break;
	}
	return FALSE;
}

//==================================================================== 
void AdjustDlgPlace(HWND hParent, HWND hDlg) {

	RECT rcParent, rcDlg;

	// Установка размеров окна hDlg
	GetClientRect(hParent, &rcParent);
	GetWindowRect(hDlg, &rcDlg);
	int width = rcDlg.right - rcDlg.left;
	int height = rcDlg.bottom - rcDlg.top;
	int dH = rcParent.bottom - height;
	ShiftWindow(hDlg, hParent, 0, 0, 0, dH);
	// Установка размеров рабочей области для окна hParent
	rcWork = rcParent;
	rcWork.left += width;
}
//////////////////////////////////////////////////////////////////////
