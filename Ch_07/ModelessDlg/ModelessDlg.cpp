//////////////////////////////////////////////////////////////////////
// ModelessDlg.cpp
#include <windows.h>

#include "KWndPlut.h"
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

	KWnd mainWnd("ModelessDlg", hInstance, nCmdShow, WndProc,
		NULL, 100, 100, 400, 300);	

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
BOOL CALLBACK ModelessDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	HWND hCtrl;
	int iCtrlID, iIndex;
	HWND hRvalue = GetDlgItem(hDlg, IDC_RED);
	HWND hGvalue = GetDlgItem(hDlg, IDC_GREEN);
	HWND hBvalue = GetDlgItem(hDlg, IDC_BLUE);

	switch (uMsg) {
	case WM_INITDIALOG:
		SetScrollRange(hRvalue, SB_CTL, 0, 255, FALSE);
		SetScrollPos(hRvalue, SB_CTL, 0, FALSE);

		SetScrollRange(hGvalue, SB_CTL, 0, 255, FALSE);
		SetScrollPos(hGvalue, SB_CTL, 0, FALSE);
		
		SetScrollRange(hBvalue, SB_CTL, 0, 255, FALSE);
		SetScrollPos(hBvalue, SB_CTL, 0, FALSE);
		return TRUE;

	case WM_VSCROLL:
		hCtrl = (HWND)lParam;
		iCtrlID = GetDlgCtrlID(hCtrl);
		iIndex = iCtrlID - IDC_RED;

		switch(LOWORD(wParam)) {
		case SB_LINEUP:
			rgb[iIndex] = max(0, rgb[iIndex] - 1);
			break;
		case SB_LINEDOWN:
			rgb[iIndex] = min(255, rgb[iIndex] + 1);
			break;
		case SB_PAGEUP:
			rgb[iIndex] -= 15;
			break;
		case SB_PAGEDOWN:
			rgb[iIndex] += 15;
			break;
		case SB_THUMBTRACK:
			rgb[iIndex] = HIWORD(wParam);
			break;
		}
		SetScrollPos(hCtrl, SB_CTL, rgb[iIndex], TRUE);

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
