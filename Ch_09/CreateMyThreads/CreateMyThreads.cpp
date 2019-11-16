//////////////////////////////////////////////////////////////////////
// CreateMyThreads.cpp
#include <windows.h>
#include <string>
using namespace std;

#include "KWnd.h"

enum UserMsg { UM_THREAD_DONE = WM_USER+1 };

struct ThreadManager {
	ThreadManager(string _name) : name(_name) { nValue = 0; }
	HWND hwndParent;
	string name;
	int nValue;
};

ThreadManager tm_A(string("Поток A"));
ThreadManager tm_B(string("Поток B"));
ThreadManager tm_C(string("Поток C"));

DWORD WINAPI ThreadFuncA(LPVOID);
DWORD WINAPI ThreadFuncB(LPVOID);
DWORD WINAPI ThreadFuncC(LPVOID);

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//====================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	KWnd mainWnd("CreateMyThreads", hInstance, nCmdShow, WndProc,
		NULL, 100, 100, 400, 160);	

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

	static HANDLE hThreadA, hThreadB, hThreadC;
	static char text[100];
	ThreadManager* pTm;
	static int y = 0;

	switch (uMsg)
	{
	case WM_CREATE:
		tm_A.hwndParent = hWnd;
		hThreadA = CreateThread(NULL, 0, ThreadFuncA, &tm_A, 0, NULL);
		if (!hThreadA)
			MessageBox(hWnd, "Error of create hThreadA", NULL, MB_OK);

		tm_B.hwndParent = hWnd;
		hThreadB = CreateThread(NULL, 0, ThreadFuncB, &tm_B, 0, NULL);
		if (!hThreadB)
			MessageBox(hWnd, "Error of create hThreadB", NULL, MB_OK);

		tm_C.hwndParent = hWnd;
		hThreadC = CreateThread(NULL, 0, ThreadFuncC, &tm_C, 0, NULL);
		if (!hThreadC)
			MessageBox(hWnd, "Error of create hThreadC", NULL, MB_OK);
		break;

	case UM_THREAD_DONE:
		pTm = (ThreadManager*)wParam;
		sprintf(text, "%s: count = %d", pTm->name.c_str(), pTm->nValue);
		y += 30;
		InvalidateRect(hWnd, NULL, FALSE);
		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		TextOut(hDC, 20, y, text, strlen(text));
		EndPaint(hWnd, &ps);
		break;

    case WM_DESTROY:
		CloseHandle(hThreadA);
		CloseHandle(hThreadB);
		CloseHandle(hThreadC);
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}
//====================================================================
DWORD WINAPI ThreadFuncA(LPVOID lpv)
{
	ThreadManager* pTm = (ThreadManager*)lpv;
	int count = 0;

	for (int i = 0; i < 100000000; ++i)  count++;
	pTm->nValue = count;
	SendMessage(pTm->hwndParent, UM_THREAD_DONE, (WPARAM)pTm, 0);

	return 0;
}
//====================================================================
DWORD WINAPI ThreadFuncB(LPVOID lpv)
{
	ThreadManager* pTm = (ThreadManager*)lpv;
	int count = 0;

	for (int i = 0; i < 50000000; ++i)  count++;
	pTm->nValue = count;
	SendMessage(pTm->hwndParent, UM_THREAD_DONE, (WPARAM)pTm, 0);

	return 0;
}
//====================================================================
DWORD WINAPI ThreadFuncC(LPVOID lpv)
{
	ThreadManager* pTm = (ThreadManager*)lpv;
	int count = 0;

	for (int i = 0; i < 20000; ++i)  count++;
	pTm->nValue = count;
	SendMessage(pTm->hwndParent, UM_THREAD_DONE, (WPARAM)pTm, 0);

	return 0;
}
//////////////////////////////////////////////////////////////////////
