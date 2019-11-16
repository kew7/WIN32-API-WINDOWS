//////////////////////////////////////////////////////////////////////
// ClientApp.cpp
#include <windows.h>
#include <stdio.h>
#include "resource.h"
#include "KWnd.h"

#define MAX_LEN_REQUEST  80
#define MAX_LEN_EVTNAME  88
#define TIMER_ID  1
#define TIMER_PERIOD  1000

HANDLE hEvtRecToServ;   // событие "Запрос к серверу от клиента"
HANDLE hEvtServIsFree;  // событие "Сервер свободен"
HANDLE hEvtServIsDone;  // событие "Сервер выполнил запрос"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//====================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	KWnd mainWnd("ClientApp", hInstance, nCmdShow, WndProc,
		MAKEINTRESOURCE(IDR_MENU1), 100, 100, 450, 150);	

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
	static char request[MAX_LEN_REQUEST];
	static char eventName[MAX_LEN_EVTNAME];
	static char text[100];
	static char msgSended[256];
	static char msgReceived[256];

	HWND hwndServer;
	static BOOL isLinkToServer;
	static BOOL bServerIsDone = FALSE;

	static HANDLE hFileMap; // объект "проекция файла"
	static PVOID pView;
	DWORD dw0, dw1;
	static int count = 0;

	switch (uMsg)
	{
	case WM_CREATE:
		isLinkToServer = FALSE;
		msgSended[0] = 0;
		msgReceived[0] = 0;
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_LINK:  // Отправка запроса на связь с сервером
			sprintf(request, "ClientApp");
			cds.lpData = &request;
			cds.cbData = strlen(request);

			hwndServer = FindWindow(NULL, "ServerApp");
			if (!hwndServer) {
				MessageBox(hWnd, "Ошибка связи!", "ClientApp", MB_OK);
				break;
			}
			SendMessage(hwndServer, WM_COPYDATA, (WPARAM)hWnd, (LPARAM)&cds);		
			break;
		}
		break;

	case WM_COPYDATA:  // Получение ответа от сервера
		pCds = (COPYDATASTRUCT*)lParam;

		// Имя разделяемого объекта-события
		strncpy(eventName, (char*)pCds->lpData, pCds->cbData);

		// Дескрипторы разделяемых объектов-событий
		hEvtRecToServ = OpenEvent(EVENT_ALL_ACCESS, FALSE, eventName);
		hEvtServIsFree = OpenEvent(EVENT_ALL_ACCESS, FALSE, "ServerIsFree");
		hEvtServIsDone = OpenEvent(EVENT_ALL_ACCESS, FALSE, "ServerIsDone");

		// Открыть файл, проецируемый в память
		hFileMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "SharedData");
		SetWindowText(hWnd, eventName);
		isLinkToServer = TRUE;
		InvalidateRect(hWnd, NULL, TRUE);

		SetTimer(hWnd, TIMER_ID, TIMER_PERIOD, NULL);
		break;

	case WM_TIMER: // Отправка "рабочих запросов" и ожидание ответа от сервера

		// Ожидание освобождения сервера
		dw0 = WaitForSingleObject(hEvtServIsFree, TIMER_PERIOD / 2);
		switch (dw0) {
		case WAIT_OBJECT_0:
			// Отображаем проекцию файла на адресное пространство процесса
			pView = MapViewOfFile(hFileMap,	FILE_MAP_WRITE, 0, 0, 0);
			// Записываем содержание "рабочего запроса"
			sprintf((PTSTR)pView, "%d\0", ++count);
			// Сообщение для вывода в окно приложения
			sprintf(msgSended, "Запрос к серверу: \t\t%s", (PTSTR)pView);
			UnmapViewOfFile(pView);

			InvalidateRect(hWnd, NULL, FALSE);
			SetEvent(hEvtRecToServ);  // освобождаем событие hEvtRecToServ

			// Ожидание события "Сервер выполнил запрос"
			dw1 = WaitForSingleObject(hEvtServIsDone, TIMER_PERIOD / 2);
			switch (dw1) {
			case WAIT_OBJECT_0:
				// Опять отображаем проекцию файла
				pView = MapViewOfFile(hFileMap,	FILE_MAP_READ, 0, 0, 0);
				// Извлекаем содержание ответа сервера
				sprintf(msgReceived, "Ответ от сервера: \t\t%s", (PTSTR)pView);
				UnmapViewOfFile(pView);
				bServerIsDone = TRUE;
				SetEvent(hEvtServIsFree); // освобождаем событие "Сервер свободен"
				InvalidateRect(hWnd, NULL, FALSE);
				break;
			case WAIT_TIMEOUT:   return 0;
			case WAIT_FAILED:
				MessageBox(hWnd, "Ошибка ожидания hEvtServIsDone", "ClientApp", MB_OK);
				return 0;
			}

		case WAIT_TIMEOUT:   return 0;
		case WAIT_FAILED:
			MessageBox(hWnd, "Ошибка ожидания hEvtServIsFree", "ClientApp", MB_OK);
			return 0;
		}

		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		
		if (isLinkToServer) {
			sprintf(text, "Установлена связь с сервером через событие %s", eventName);
			TextOut(hDC, 20, 20, text, strlen(text)); 
			TabbedTextOut(hDC, 20, 40, msgSended, strlen(msgSended), 0, NULL, 20); 
		}
		if (bServerIsDone) {
			bServerIsDone = FALSE;
			TabbedTextOut(hDC, 20, 60, msgReceived, strlen(msgReceived), 0, NULL, 20); 
		}

		EndPaint(hWnd, &ps);
		break;

    case WM_DESTROY:
		UnmapViewOfFile(pView);
		CloseHandle(hFileMap);
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////