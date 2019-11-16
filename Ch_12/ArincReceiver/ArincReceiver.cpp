//////////////////////////////////////////////////////////////////////
// ArincReceiver.cpp
#include <windows.h>
#include <commctrl.h>
#include <Mmsystem.h>
#include <stdio.h>
#include <math.h>

#include "resource.h"
#include "KWndEx.h"
#include "KTimer.h"
#include "KDibSection.h"

#define ID_STATUSBAR 201

#define PACK_PERIOD  5     // � �������������

#define LEFT_MARGIN  -683  // angle = -60 ��������
#define RIGHT_MARGIN  683  // angle = +60 ��������
#define Pi  3.141592653589793

HDC g_hDC;
HWND hwndStatusBar;

double sweepSpeedDue; // ��������� �������� ��������� (�������� � �������)
double speedWithPackPeriod; // �������� ��������� ��� ������� PACK_PERIOD
int sieveCoeff;    // ����������� �����������/����������� ����� ��������� 

double sweepTimeDue;  // ��������� ����� ��������� ������� (���)
double sweepTimeFact; // ����������� ����� ��������� ������� (���)

int scanAngle;      // ��� ���� ������������ � �������� ������
double digitWeight; // ��� ������� ���� scanAngle

COLORREF cell[512]; // ������ ������ ����� ���������
int X0, Y0;         // ������ ������� ��������� ��� ���������
double x, y;        // ������� ��������� ������ �����

KTimer timer;

typedef enum { FORTH, BACK } DIR;
DIR direction = FORTH;    // ����������� ������������

KDibSection dibSect;

void UpdateStatusBar();
void CALLBACK TimerFunc(UINT, UINT, DWORD, DWORD, DWORD);
void GetPacket();         // ��������� ������ �� ������ �����
void GetNextAngle();      // ���������� ���������� �������� ���� ����
void SetColorBeamForth(); // �������� ������������ ������� cell
	                      //   ��� ������� ������������
void SetColorBeamBack();  // �������� ������������ ������� cell
	                      //   ��� ��������� ������������
void DrawBeam();          // ����� ���� �� �����

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//==================================================================== 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	KWndEx mainWnd("ArincReceiver", hInstance, nCmdShow, WndProc,
		MAKEINTRESOURCE(IDR_MENU1), 100, 100, 900, 600);	

	while (GetMessage(&msg, NULL, 0, 0))  {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
//==================================================================== 
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HMENU hMenu; // ���������� �������� ����
	RECT rect, rcSB;
	int aWidths[4];
	static MMRESULT mmTimer;

	switch (uMsg)
	{
	case WM_CREATE:
		hMenu = GetMenu(hWnd);
		SetClassLong(hWnd, GCL_HBRBACKGROUND, (LONG)CreateSolidBrush(RGB(100, 100, 100)));

		sweepSpeedDue = 90;
		CheckMenuRadioItem(GetSubMenu(hMenu, 0), IDM_SPEED_15,
			IDM_SPEED_90, IDM_SPEED_90, MF_BYCOMMAND);

		// �������� ������ ���������
		hwndStatusBar = CreateStatusWindow(WS_CHILD | WS_VISIBLE, "", hWnd, ID_STATUSBAR);
		aWidths [0] = 205;
		aWidths [1] = 355;
		aWidths [2] = 610;
		aWidths [3] = -1;
		SendMessage(hwndStatusBar, SB_SETPARTS, 4, (LPARAM)aWidths);
		SendMessage(hwndStatusBar, SB_SIMPLE, FALSE, 0);
		
		digitWeight = 360.0 / 4096;
		speedWithPackPeriod = digitWeight / (PACK_PERIOD / 1000.);
		sieveCoeff = ceil(sweepSpeedDue / speedWithPackPeriod);

		UpdateStatusBar();

		GetClientRect(hWnd, &rect);
		GetWindowRect(hwndStatusBar, &rcSB);
		X0 = rect.right / 2;
		Y0 = rect.bottom - (rcSB.bottom - rcSB.top);

		// �������� ���������� ��� ��������� ���������
		g_hDC = GetDC(hWnd);
		SetColorBeamForth();

		// �������� DIB-������
		dibSect.Create(g_hDC, rect.right, Y0);
		break;

	case WM_SIZE:
		SendMessage (hwndStatusBar, WM_SIZE, wParam, lParam);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_SPEED_15:
			sweepSpeedDue = 15;
			CheckMenuRadioItem(GetSubMenu(hMenu, 0), IDM_SPEED_15,
				IDM_SPEED_90, LOWORD(wParam), MF_BYCOMMAND);
			break;
		case IDM_SPEED_30:
			sweepSpeedDue = 30;
			CheckMenuRadioItem(GetSubMenu(hMenu, 0), IDM_SPEED_15,
				IDM_SPEED_90, LOWORD(wParam), MF_BYCOMMAND);
			break;
		case IDM_SPEED_60:
			sweepSpeedDue = 60;
			CheckMenuRadioItem(GetSubMenu(hMenu, 0), IDM_SPEED_15,
				IDM_SPEED_90, LOWORD(wParam), MF_BYCOMMAND);
			break;
		case IDM_SPEED_90:
			sweepSpeedDue = 90;
			CheckMenuRadioItem(GetSubMenu(hMenu, 0), IDM_SPEED_15,
				IDM_SPEED_90, LOWORD(wParam), MF_BYCOMMAND);
			break;

		case IDM_START:
			timer.Start();
			mmTimer = timeSetEvent(PACK_PERIOD, 0, TimerFunc,
				reinterpret_cast<DWORD>(hWnd), TIME_PERIODIC);
			if (!mmTimer)
				MessageBox(hWnd, "Error of timeSetEvent!", NULL, MB_OK | MB_ICONSTOP);
			break;

		case IDM_STOP:
			timeKillEvent(mmTimer);
			break;
		}

		sieveCoeff = ceil(sweepSpeedDue / speedWithPackPeriod);
		UpdateStatusBar();
		break;

    case WM_DESTROY:
		ReleaseDC(hWnd, g_hDC);
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}
//====================================================================
void UpdateStatusBar()
{
	char text[100];
	sprintf(text, "�������� ������������:  %.0f ����./���", sweepSpeedDue); 
	SendMessage(hwndStatusBar, SB_SETTEXT, 0, (LPARAM)text);

	sprintf(text, "�����. �������. �����:  %d", sieveCoeff); 
	SendMessage(hwndStatusBar, SB_SETTEXT, 1, (LPARAM)text);

	sweepTimeDue = 2.*(RIGHT_MARGIN-LEFT_MARGIN)*(PACK_PERIOD/1000.)/sieveCoeff;
	sprintf(text, "��������� ����� ��������� �������:  %.2f ���", sweepTimeDue); 
	SendMessage(hwndStatusBar, SB_SETTEXT, 2, (LPARAM)text);

	sprintf(text, "����������� ����� ��������� �������:  %.2f ���", sweepTimeFact); 
	SendMessage(hwndStatusBar, SB_SETTEXT, 3, (LPARAM)text);
}
//====================================================================
void CALLBACK TimerFunc(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	// ���������� ����-��������� 
	HWND hwndOwner = reinterpret_cast<HWND>(dwUser);
	// (� ������ ��������� �� ������������, �� ����� ������������
	// � �������� ��������� ��� �������� ��������� ����-���������)

	POINT ppt[4];  // ������ ����� ��� �������� ������� ���������
	// ���������� ����� ppt[0] � ppt[3]
	ppt[0].y = ppt[3].y = Y0;
	if (direction == FORTH) { ppt[0].x = X0 - 1;  ppt[3].x = X0 + 1; }
	if (direction == BACK)  { ppt[0].x = X0 + 1;  ppt[3].x = X0 - 1; }

	GetPacket(); // ��������� ������ �� ������ �����
	DrawBeam();  // ����� ���� �� ����������� ������

	// ���������� ����� ppt[1]
	ppt[1].y = y;
	if (direction == FORTH) ppt[1].x = x - 1;
	if (direction == BACK)  ppt[1].x = x + 1; 

	// ����������� �����
	for (int i = 1; i < sieveCoeff; ++i) {
		GetNextAngle();
		DrawBeam();
	}

	// ���������� ����� ppt[2]
	ppt[2].y = y;
	if (direction == FORTH) ppt[2].x = x + 1;; 
	if (direction == BACK)  ppt[2].x = x - 1;

	// �������� ������� ���������
	HRGN hRgn = CreatePolygonRgn(ppt, 4, WINDING);
	SelectClipRgn(g_hDC, hRgn);

	// ����������� ����������� �� ����� (� ������ ������� ���������)
	dibSect.Draw(g_hDC);

	DeleteObject(hRgn);
}

//====================================================================
void GetPacket() {
	GetNextAngle();
}

//====================================================================
void GetNextAngle() {
	char text[100];

	if (direction == FORTH)
		if (++scanAngle >= RIGHT_MARGIN)  { 
			direction = BACK;
			SetColorBeamBack();

			sweepTimeFact = timer.GetTime() / 1000;
			timer.Start();
			sprintf(text, "����������� ����� ��������� �������:  %.2f ���", sweepTimeFact); 
			SendMessage(hwndStatusBar, SB_SETTEXT, 3, (LPARAM)text);
		}
	if (direction == BACK)
		if (--scanAngle <= LEFT_MARGIN) {
			direction = FORTH;
			SetColorBeamForth();
		}
}
//====================================================================
void SetColorBeamForth() {
	for (int i = 0; i < 512; ++i) {
		if (i < 128)       cell[i] = RGB(255, 0, 0);
		else if (i < 256)  cell[i] = RGB(0, 255, 0);
		else if (i < 384)  cell[i] = RGB(255, 255, 0);
		else               cell[i] = RGB(0, 0, 255);
	}
}
//====================================================================
void SetColorBeamBack() {
	for (int i = 0; i < 512; ++i) {
		if (i < 128)       cell[i] = RGB(0, 0, 255);
		else if (i < 256)  cell[i] = RGB(255, 255, 0);
		else if (i < 384)  cell[i] = RGB(0, 255, 0);
		else               cell[i] = RGB(255, 0, 0);
	}
}
//====================================================================
void DrawBeam() {
	x = X0;
	y = Y0;

	double angle = scanAngle * digitWeight;
	double C = cos(Pi * (90 - angle) / 180);
	double S = sin(Pi * (90 - angle) / 180);

	for (int k = 0; k < 512; ++k) {
		x += C;	 y -= S;
		// ����� ����� �� ����������� DIB-������
		dibSect.SetPixel((int)x, (int)y, cell[k]);
	}
}
//////////////////////////////////////////////////////////////////////
