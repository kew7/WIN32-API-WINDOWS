//////////////////////////////////////////////////////////////////////
// ProgressBar.cpp
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <fstream>
using namespace std;

#include "KWndEx.h"
#include "resource.h"

#define ID_STATUSBAR  201
#define N_PARTS    2

#define DATA_CHUNK  64
#define FILE_NAME "ProgressBar.cpp"

HWND hwndStatusBar;
HWND hwndProgressBar;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//==================================================================== 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	KWndEx mainWnd("ProgressBar", hInstance, nCmdShow, WndProc,
		MAKEINTRESOURCE(IDR_MENU1), 100, 100, 400, 200);	

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
	char text[100];
	RECT rect, r1;

	int aWidths[N_PARTS];

	ifstream file;
	static int size;       // размер файла в байтах
	static int range;      // диапазон для индикатора процесса
	static int nBytesRead; // число прочитанных байт

	double percentage;
	char buf[DATA_CHUNK];

	static int i;
	MSG message;

	switch (uMsg)
	{
	case WM_CREATE:
		// Создание строки состояния
		hwndStatusBar = CreateStatusWindow(WS_CHILD | WS_VISIBLE, "", hWnd, ID_STATUSBAR);
		// Увеличиваем высоту строки состояния
		SendMessage(hwndStatusBar, SB_SETMINHEIGHT, 24, 0);

		aWidths [0] = 50;
		aWidths [1] = -1;
		SendMessage(hwndStatusBar, SB_SETPARTS, N_PARTS, (LPARAM)aWidths);
		// Извлекаем координаты 1-го поля 
		SendMessage(hwndStatusBar, SB_GETRECT, 1, (LPARAM)&r1);

		// Создание индикатора процесса
		hwndProgressBar = CreateWindow(PROGRESS_CLASS, NULL, WS_CHILD | WS_VISIBLE,
			r1.left + 3, r1.top + 3, r1.right - r1.left, r1.bottom - r1.top,
			hwndStatusBar, NULL, NULL, NULL);
		break;

	case WM_SIZE:
		SendMessage (hwndStatusBar, WM_SIZE, wParam, lParam);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_LOAD_FILE:
			// Открывем файл
			file.open(FILE_NAME, ios::in | ios::binary);
			if (!file) {
				MessageBox(hWnd, "Нет файла", NULL, MB_OK | MB_ICONSTOP);
				break;
			}
			// Определяем размер файла
			file.seekg(0, ios::end);
			size = file.tellg();
			file.seekg(0, ios::beg);

			// Настраиваем параметры индикатора процесса
			range = size / DATA_CHUNK;
			SendMessage(hwndProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, range));
			SendMessage(hwndProgressBar, PBM_SETSTEP, 1, 0);
			SendMessage(hwndProgressBar, PBM_SETPOS, 0, 0);
			InvalidateRect(hWnd, NULL, FALSE);
							
			nBytesRead = 0;
			// Загрузка файла
			while (!file.eof()) {

				// Чтение порции данных
				file.read(buf, DATA_CHUNK);
				nBytesRead += DATA_CHUNK;

				if (file.eof())  break;
		
				// ... Здесь возможен вызов процедуры для обработки порции данных

				// Отображение процента выполненной операции
				percentage = 100.0 * nBytesRead / size;
				sprintf(text, "  %.0f %%", percentage); 
				SendMessage(hwndStatusBar, SB_SETTEXT, 0, (LPARAM)text);

				SendMessage(hwndProgressBar, PBM_STEPIT, 0, 0);
				Sleep(100);
		
				// Предотвращение зависания приложения
				if(PeekMessage(&message, hWnd, 0, 0, PM_REMOVE)) {
					TranslateMessage(&message);
					DispatchMessage(&message);
				}

			}

			SendMessage(hwndStatusBar, SB_SETTEXT, 0, (LPARAM)" 100 %");
			MessageBox(hWnd, "Файл загружен.", "Завершение операции", MB_OK | MB_ICONINFORMATION);
			// Сброс индикатора процесса в исходное положение
			SendMessage(hwndProgressBar, PBM_SETPOS, 0, 0);
			SendMessage(hwndStatusBar, SB_SETTEXT, 0, (LPARAM)" Ready");

			break;
		case IDM_EXIT:
			SendMessage(hWnd, WM_DESTROY, 0, 0);
			break;

		default:
			break;
		}
		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		GetClientRect(hWnd, &rect);
		sprintf(text, "size = %d,  DATA_CHUNK = %d,  range = %d",
			size, DATA_CHUNK, range);
		DrawText(hDC, text, -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
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
