//////////////////////////////////////////////////////////////////////
// Main.cpp
#include <windows.h>
#include <Mmsystem.h>
#include <fstream>
#include <iomanip>
using namespace std;

#include "KWnd.h"
#include "KTimer.h"

KTimer timer1;
KTimer timer2;
KTimer timer3;

#define N  5000000

int dTicks[N]; //  ����� ������ �������� TSC ��� ������������ �������� �������
double grossWorkQuant[N]; // ������ ������� ������ ���������� (������)
ofstream flog;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//====================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	KWnd mainWnd("������������ ������ KTimer", hInstance, nCmdShow, WndProc);	

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
	int i, No = 0;
	char text[200];
	double netWorkQuant; // ����� ������� ������ ���������� (�����)
	double sleepQuant;   // ����� ������� "������" ����������
	double totalTime;    // ����� ����� ���������� ����� for

	switch (uMsg)
	{
	case WM_CREATE:
		flog.open("flog.txt");
		timer3.Start();
		timer2.Start();

		for (i = 0; i < N; ++i) {
			timer1.Start();
			// ... ���� ���������� �� ������������� �������
			dTicks[i] = timer1.GetTick();
			if (dTicks[i] > 0) {
				grossWorkQuant[i] = timer2.GetTime();
				timer2.Start();
			}
		}
		totalTime = timer3.GetTime();

		flog << "���������� ��������� dTicks �� ���������� �������� 0\n";
		flog << "-----------------------------------------------------\n";
		flog << "      No         i    dTicks   sleepQuant  workQuant\n";
		flog << "                                  (��)        (��)\n";
		flog << "-----------------------------------------------------\n";
		flog.flags(ios::fixed);

		for (i = 0; i < N; ++i) {
			if (dTicks[i]) {
				No++;
				sleepQuant = (double)dTicks[i] / timer1.GetTickPerUsec() / 1000.;
				netWorkQuant = grossWorkQuant[i] - sleepQuant;
				flog << setw(8) << No << setw(10) << i << setw(10)
					<< dTicks[i] << setw(12) << setprecision(5) << sleepQuant
					<< setw(10) << setprecision(3) << netWorkQuant << endl; 
			}
		}
		flog << "-----------------------------------------------------\n";
		flog << "����� ����� ����������:   " << totalTime << " ��" << endl;
		
		flog.close();		

		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		sprintf(text, "overhead = %d,   CPU speed: %d Mhz\0\n",
			timer1.GetOverhead(), timer1.GetTickPerUsec());
		TextOut(hDC, 20, 10, text, strlen(text));
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
