//////////////////////////////////////////////////////////////////////
// ServerApp.cpp
#include <windows.h>
#include <stdio.h>
#include "KWnd.h"

#define MAX_STR_SIZE  80
#define MAX_N  10  // максимальное количество клиентов
int N = 0;         // текущее количество клиентов

HANDLE hEvtRecToServ[MAX_N]; // массив событий "рабочий запрос от клиента"
char eventName[MAX_N][MAX_STR_SIZE + 8];  // массив имен событий 

HANDLE hEvtServIsExist; // событие "Сервер уже запущен"
HANDLE hEvtServIsFree;  // событие "Сервер свободен"
HANDLE hEvtServIsDone;  // событие "Сервер выполнил рабочий запрос"
HANDLE hFileMap = NULL; // объект "проекция файла"

struct ThreadManager {
	HWND hwndParent;
};
ThreadManager tm;  // структура для связи с потоком

DWORD WINAPI ThreadFunc(LPVOID);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//====================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	KWnd mainWnd("ServerApp", hInstance, nCmdShow, WndProc,
				 NULL, 100, 100, 450, 100);	

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

	COPYDATASTRUCT* pCds;
	COPYDATASTRUCT  cds;
	static char request[MAX_STR_SIZE];
	char suffix[8];

	HWND hwndClient;
	static HANDLE hThread;
	HWND* pHwnd = &hWnd;

	int i;

	switch (uMsg)
	{
	case WM_CREATE:
		hEvtServIsExist = OpenEvent(EVENT_ALL_ACCESS, FALSE, "Server");
		if (hEvtServIsExist) {
			MessageBox(hWnd, "Сервер уже выполняется. 2-й экземпляр запрещен.",
				"ServerApp", MB_OK | MB_ICONSTOP | MB_SYSTEMMODAL);
			PostQuitMessage(0);
		}
		else
			hEvtServIsExist = CreateEvent(NULL, FALSE, FALSE, "Server");
		break;

	case WM_COPYDATA:  // прием запроса от клиента на связь с сервером

		if (N == MAX_N) {
			MessageBox(hWnd, "Сервер перегружен. В доступе отказано",
				"ServerApp", MB_OK | MB_ICONSTOP | MB_SYSTEMMODAL);
			break;
		}
		// Извлечение сообщения
		pCds = (COPYDATASTRUCT*)lParam;
		strncpy(request, (char*)pCds->lpData, pCds->cbData);
		strcpy(eventName[N], request);
		sprintf(suffix, "_%d", N);
		strcat(eventName[N], suffix);

		// Создание события hEvtRecToServ[N] - "запрос N-го клиента"
		hEvtRecToServ[N] = CreateEvent(NULL, FALSE, FALSE, eventName[N]);		
		
		if (!N) {
			// Инициализация массива hEvtRecToServ
			for (i = 0; i < MAX_N; ++i)
				hEvtRecToServ[i] = hEvtRecToServ[0];

			// Создание файла, проецируемого в память
			hFileMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, 
					PAGE_READWRITE, 0, 4 * 1024, "SharedData");
			// Создание событий "Сервер свободен" и "Сервер выполнил рабочий запрос"
			hEvtServIsFree = CreateEvent(NULL, FALSE, TRUE, "ServerIsFree");
			hEvtServIsDone = CreateEvent(NULL, FALSE, FALSE, "ServerIsDone");
			// Запуск потока для обработки запросов клиентов
			tm.hwndParent = hWnd;
			hThread = CreateThread(NULL, 0, ThreadFunc, &tm, 0, NULL);
		}

		// Отправка обратного сообщения клиенту
		hwndClient = FindWindow(NULL, request);
		cds.dwData = N;
		cds.lpData = &eventName[N];
		cds.cbData = strlen(eventName[N]);
		SendMessage(hwndClient, WM_COPYDATA, (WPARAM)hWnd, (LPARAM)&cds);

		if (N < MAX_N)   N++;
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		if (N) {
			char text[40];
			sprintf(text, "Количество обслуживаемых клиентов: %d", N);
			TextOut(hDC, 10, 20, text, strlen(text));
		}
		EndPaint(hWnd, &ps);
		break;

    case WM_DESTROY:
		CloseHandle(hFileMap);
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}
//====================================================================
DWORD WINAPI ThreadFunc(LPVOID lpv)
{
	ThreadManager* pTm = (ThreadManager*)lpv;
	HWND hParent = pTm->hwndParent;
	static PVOID pView;
	char suffix[40];
	int k;
	char text[100];
	DWORD dw;

	while (1) {
		// Ожидание "рабочего запроса" от клиента
		dw = WaitForMultipleObjects(MAX_N, hEvtRecToServ, FALSE, INFINITE);
		switch (dw) {
		case WAIT_FAILED:
			MessageBox(hParent, "Ошибка вызова WaitForMultipleObjects", "ServerApp", MB_OK);
			return 0;

		default:
			k = dw - WAIT_OBJECT_0;  // индекс клиента
			// Отображаем проекцию файла на адресное пространство процесса
			pView = MapViewOfFile(hFileMap, FILE_MAP_WRITE, 0, 0, 0);
			// Извлекаем содержание запроса в буфер text
			strcpy(text, (PTSTR)pView);
			// Добавляем к нему "суффикс", содержащий имя клиента
			sprintf(suffix, " - %s\0",	eventName[k]);
			strcat(text, suffix);
			// Помещаем сформированную запись обратно в проекцию файла
			strcpy((PTSTR)pView, text);
			UnmapViewOfFile(pView);
			// Освобождаем событие hEvtServIsDone
			SetEvent(hEvtServIsDone);
			break;
		}
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////
