//////////////////////////////////////////////////////////////////////
// MtClient.cpp
#include <windows.h>
#include "MyLib.h"

DWORD WINAPI ThreadFuncA(LPVOID);
DWORD WINAPI ThreadFuncB(LPVOID);

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//====================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	HANDLE hThreadA = CreateThread(NULL, 0, ThreadFuncA, 0, 0, NULL);
	HANDLE hThreadB = CreateThread(NULL, 0, ThreadFuncB, 0, 0, NULL);

	MessageBox(NULL, "Завершить основной поток?", "WinMain", MB_OK);
	return 0;
}
//====================================================================
DWORD WINAPI ThreadFuncA(LPVOID lpv)
{
	Print("Поток А: Привет!");
	Print("Поток А: Эксперимент удался!");
	Print("Поток А: Пока!");
	return 0;
}
//====================================================================
DWORD WINAPI ThreadFuncB(LPVOID lpv)
{
	Print("Поток Б: Hello!");
	Print("Поток Б: The experiment was a success!");
	Print("Поток Б: By!");
	return 0;
}
