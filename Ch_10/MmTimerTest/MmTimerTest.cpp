//////////////////////////////////////////////////////////////////////
// MmTimerTest.cpp
#include <windows.h>
#include <stdio.h>
#include <Mmsystem.h>
#include <fstream>
using namespace std;

#include "KWnd.h"
#include "KTimer.h"

ofstream flog;
KTimer timer;

enum UserMsg { UM_READY = WM_USER+1 };
#define TIME_PERIOD 1

//#define N  1000
#define N  100

double realTimeInterval[N];

void CALLBACK TimeProc(UINT, UINT, DWORD, DWORD, DWORD);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//====================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	flog.open("flog.txt");
	MSG msg;
	KWnd mainWnd("MM timer - test", hInstance, nCmdShow, WndProc);	

	while (GetMessage(&msg, NULL, 0, 0))  {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	flog.precision(3);
	for (int i = 0; i < N; ++i)
		flog << realTimeInterval[i] << endl;
	flog.close();
	return msg.wParam;
}

//====================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hDC;
	PAINTSTRUCT ps;
	char text[200];
	int i;
	const int i0 = 30;  // номер первого обрабатывемого измерения 
	double minT, maxT, amount;
	static MMRESULT mmResult;

	switch (uMsg)
	{
	case WM_CREATE:

//timeBeginPeriod(1);     // #1

		mmResult = timeSetEvent(TIME_PERIOD, 0, TimeProc, reinterpret_cast<DWORD>(hWnd), TIME_PERIODIC);
		if (!mmResult)
			MessageBox(hWnd, "Мультимедийный таймер не создан", "Error", MB_OK);
		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);

		// Определяем min и max значения
		minT = maxT = amount = realTimeInterval[i0];
		for (i = i0 + 1; i < N; ++i) {
			if (realTimeInterval[i] < minT)
				minT = realTimeInterval[i];
			if (realTimeInterval[i] > maxT)
				maxT = realTimeInterval[i];
			amount += realTimeInterval[i];
		}
		sprintf(text, "minT = %.3f,  maxT = %.3f, average = %.3f\0",
			minT, maxT, amount / (N - i0));
		TextOut(hDC, 10, 10, text, strlen(text));

		EndPaint(hWnd, &ps);
		break;

	case UM_READY:
		MessageBox(hWnd, "Готово", "Завершение задания", MB_OK);
		SendMessage(hWnd, WM_DESTROY, 0, 0);
		break;

    case WM_DESTROY:
		timeKillEvent(mmResult);

//timeEndPeriod(1);     // #2

		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}
//====================================================================
void CALLBACK TimeProc(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	static int count = -1;
	HWND hwnd = reinterpret_cast<HWND>(dwUser);

	if (count >= 0 && count < N)
		realTimeInterval[count] = timer.GetTime();	
	timer.Start();
	count++;
	if (count == N) {
		InvalidateRect(hwnd, NULL, TRUE);
		SendMessage(hwnd, UM_READY, 0, 0);
	}
}
//////////////////////////////////////////////////////////////////////
