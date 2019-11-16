//////////////////////////////////////////////////////////////////////
// SetTimer1Test.cpp
#include <windows.h>
#include <stdio.h>

#include "KWnd.h"
#include "KTimer.h"

KTimer timer;

#define ID_TIMER 1
#define TIME_PERIOD 100

//#define N  1000   // приготовьтесь долго ждать
#define N  100

double realTimeInterval[N];

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//====================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	KWnd mainWnd("SetTimer1 - test", hInstance, nCmdShow, WndProc);	

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
	static int x, y;
	char text[200];
	int i;
	double minT, maxT, amount;
	static int count = -1;

	switch (uMsg)
	{
	case WM_CREATE:
		SetTimer(hWnd, ID_TIMER, TIME_PERIOD, NULL);
		break;

	case WM_TIMER:
		if (count >= 0 && count < N)
			realTimeInterval[count] = timer.GetTime();	
		timer.Start();
		count++;
		if (count == N)  InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);

		sprintf(text, "Фактическая величина задержки для TIME_PERIOD = %d", TIME_PERIOD);
		TextOut(hDC, 0, 20, text, strlen(text)); 

		x = 15;  y = 40;

		for (i = 0; i < 20; ++i) {
			sprintf(text, "%.2f\0", realTimeInterval[i]);
			TextOut(hDC, x, y, text, strlen(text));
			y += 16;
		}

		// Определяем min и max значения
		minT = maxT = amount = realTimeInterval[0];
		for (i = 1; i < N; ++i) {
			if (realTimeInterval[i] < minT)
				minT = realTimeInterval[i];
			if (realTimeInterval[i] > maxT)
				maxT = realTimeInterval[i];
			amount += realTimeInterval[i];
		}
		sprintf(text, "minT = %.2f,  maxT = %.2f, average = %.2f\0",
			minT, maxT, amount / N);
		TextOut(hDC, x, y, text, strlen(text));

		EndPaint(hWnd, &ps);
		break;

    case WM_DESTROY:
		KillTimer(hWnd, ID_TIMER);
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////
