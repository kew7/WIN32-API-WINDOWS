//////////////////////////////////////////////////////////////////////
// Spinner.cpp
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

	KWndEx mainWnd("Spinner", hInstance, nCmdShow, WndProc,
		NULL, 100, 100, 400, 260);	

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
	static HWND hSpinRed;
	static HWND hSpinGreen;
	static HWND hSpinBlue;
	static HWND hStaticRed;
	static HWND hStaticGreen;
	static HWND hStaticBlue;

	switch (uMsg) {
	case WM_INITDIALOG:
		// Определяем дескрипторы элементов управления Spin
		hSpinRed = GetDlgItem(hDlg, IDC_SPIN_RED);
		hSpinGreen = GetDlgItem(hDlg, IDC_SPIN_GREEN);
		hSpinBlue = GetDlgItem(hDlg, IDC_SPIN_BLUE);
		hStaticRed = GetDlgItem(hDlg, IDC_STATIC_RED);
		hStaticGreen = GetDlgItem(hDlg, IDC_STATIC_GREEN);
		hStaticBlue = GetDlgItem(hDlg, IDC_STATIC_BLUE);

		// Инициализация элементов управления Spin
		SendMessage(hSpinRed, UDM_SETRANGE, TRUE, MAKELPARAM(255, 0));
		SendMessage(hSpinGreen, UDM_SETRANGE, TRUE, MAKELPARAM(255, 0));
		SendMessage(hSpinBlue, UDM_SETRANGE, TRUE, MAKELPARAM(255, 0));
		return TRUE;

	case WM_VSCROLL:
		// Получаем текущие позиции элементов управления Spin		
		rgb[0] = SendMessage(hSpinRed, UDM_GETPOS, 0, 0);
		rgb[1] = SendMessage(hSpinGreen, UDM_GETPOS, 0, 0);
		rgb[2] = SendMessage(hSpinBlue, UDM_GETPOS, 0, 0);

		// Сообщение родительскому окну об изменении цвета фона
		SendMessage(GetParent(hDlg), UM_CHANGE, 0, 0);
		break;

	case WM_COMMAND:
		// Обновление фона основного окна после ввода
		// через приятельское окно редактирования
		if (HIWORD(wParam) == EN_CHANGE)
			SendMessage(hDlg, WM_VSCROLL, 0, 0);
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
