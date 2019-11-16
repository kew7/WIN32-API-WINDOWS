//////////////////////////////////////////////////////////////////////
// MyMtLib.cpp

#include <stdio.h>
#include "MyMtLib.h"
#include "KWndEx.h"
#include "resource.h"

struct GlobData {
	int x;
	int y;
	int count;
	char title[100];
	char buffer[256];
};

HINSTANCE hInstDll;
DWORD gdwTlsIndex;

BOOL CALLBACK PrintBoxDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

EXPORT void Print(LPCTSTR text)
{
	// Отладочный вывод
	DWORD threadID = GetCurrentThreadId();
	TRACE("Print: вызов из потока %X, параметр text = '%s'\n",
		threadID, text);
	
	// Получаем адрес памяти, связанный с индексом gdwTlsIndex 
	GlobData* pGlobData = (GlobData*)TlsGetValue(gdwTlsIndex);
	pGlobData->count++;

	sprintf(pGlobData->title, "MyMtLib: Print: вызов %d", pGlobData->count);
	strncpy(pGlobData->buffer, text, 99);

	DialogBox(hInstDll, MAKEINTRESOURCE(IDD_PRINT_BOX), NULL, PrintBoxDlgProc);
}

EXPORT void InitMyMtLib(int x, int y)
{	
	// Получаем адрес памяти, связанный с индексом gdwTlsIndex 
	GlobData* pGlobData = (GlobData*)TlsGetValue(gdwTlsIndex);

	pGlobData->x = x;
	pGlobData->y = y;
	pGlobData->count = 0;
}

BOOL CALLBACK PrintBoxDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	RECT r;
	GlobData* pGlobData = (GlobData*)TlsGetValue(gdwTlsIndex);

	switch (uMsg) {

	case WM_INITDIALOG:
		SetWindowText(hDlg, pGlobData->title);
		SetWindowText(GetDlgItem(hDlg, IDC_EDIT1), pGlobData->buffer);
		
		GetWindowRect(hDlg, &r);
		MoveWindow(hDlg, pGlobData->x, pGlobData->y,
			r.right - r.left, r.bottom - r.top, TRUE);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			EndDialog(hDlg, 0);
			return TRUE;
		}
		break;
	}
	return FALSE;	
}


BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	GlobData* pGlobData; 
	hInstDll = hinstDLL;
	DWORD threadID;

	switch (fdwReason) {
	
	case DLL_PROCESS_ATTACH:

		// Отладочный вывод
		TRACE("DllMain: подключение DLL, hinstDLL = %X\n", hinstDLL);

		// Размещаем TLS-индекс
 		if ((gdwTlsIndex = TlsAlloc()) == 0xFFFFFFFF) 
			return FALSE; 

		TRACE("DllMain: получен индекс gdwTlsIndex = %d первого свободного слота\n", gdwTlsIndex);
		break;

	case DLL_THREAD_ATTACH:  // появился новый поток

		// Отладочный вывод
		threadID = GetCurrentThreadId();
		TRACE("DllMain: создание процессом нового потока, threadID = %X\n", threadID);
			
		if (TlsGetValue(gdwTlsIndex) == NULL) // если память в TLS еще не выделена
		{
			// Выделяем память в куче
			pGlobData = (GlobData*)LocalAlloc(LPTR, sizeof(GlobData)); 
			// Связываем TLS-индекс потока с адресом памяти pGlobData
			TlsSetValue(gdwTlsIndex, pGlobData);
			// Отладочный вывод
			TRACE("DllMain: выделена память в куче с адресом %X\n", pGlobData);
			TRACE("DllMain: адрес %X записан в слот с индексом %d\n", pGlobData, gdwTlsIndex);
		} 
		break;

	case DLL_THREAD_DETACH:

		// Отладочный вывод
		threadID = GetCurrentThreadId();
		TRACE("DllMain: завершение потока, threadID = %X\n", threadID);

		// Освобождаем размещенную память для этого потока
		pGlobData = (GlobData*)TlsGetValue(gdwTlsIndex); 
		if (pGlobData != NULL)
			LocalFree((HLOCAL)pGlobData); 
		break;

	case DLL_PROCESS_DETACH:

		// Отладочный вывод
		TRACE("DllMain: отключение DLL\n");

		// Освобождаем TLS-index.
		TlsFree(gdwTlsIndex); 
		break;
	}

	return TRUE;
}
