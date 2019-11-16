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
	// ���������� �����
	DWORD threadID = GetCurrentThreadId();
	TRACE("Print: ����� �� ������ %X, �������� text = '%s'\n",
		threadID, text);
	
	// �������� ����� ������, ��������� � �������� gdwTlsIndex 
	GlobData* pGlobData = (GlobData*)TlsGetValue(gdwTlsIndex);
	pGlobData->count++;

	sprintf(pGlobData->title, "MyMtLib: Print: ����� %d", pGlobData->count);
	strncpy(pGlobData->buffer, text, 99);

	DialogBox(hInstDll, MAKEINTRESOURCE(IDD_PRINT_BOX), NULL, PrintBoxDlgProc);
}

EXPORT void InitMyMtLib(int x, int y)
{	
	// �������� ����� ������, ��������� � �������� gdwTlsIndex 
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

		// ���������� �����
		TRACE("DllMain: ����������� DLL, hinstDLL = %X\n", hinstDLL);

		// ��������� TLS-������
 		if ((gdwTlsIndex = TlsAlloc()) == 0xFFFFFFFF) 
			return FALSE; 

		TRACE("DllMain: ������� ������ gdwTlsIndex = %d ������� ���������� �����\n", gdwTlsIndex);
		break;

	case DLL_THREAD_ATTACH:  // �������� ����� �����

		// ���������� �����
		threadID = GetCurrentThreadId();
		TRACE("DllMain: �������� ��������� ������ ������, threadID = %X\n", threadID);
			
		if (TlsGetValue(gdwTlsIndex) == NULL) // ���� ������ � TLS ��� �� ��������
		{
			// �������� ������ � ����
			pGlobData = (GlobData*)LocalAlloc(LPTR, sizeof(GlobData)); 
			// ��������� TLS-������ ������ � ������� ������ pGlobData
			TlsSetValue(gdwTlsIndex, pGlobData);
			// ���������� �����
			TRACE("DllMain: �������� ������ � ���� � ������� %X\n", pGlobData);
			TRACE("DllMain: ����� %X ������� � ���� � �������� %d\n", pGlobData, gdwTlsIndex);
		} 
		break;

	case DLL_THREAD_DETACH:

		// ���������� �����
		threadID = GetCurrentThreadId();
		TRACE("DllMain: ���������� ������, threadID = %X\n", threadID);

		// ����������� ����������� ������ ��� ����� ������
		pGlobData = (GlobData*)TlsGetValue(gdwTlsIndex); 
		if (pGlobData != NULL)
			LocalFree((HLOCAL)pGlobData); 
		break;

	case DLL_PROCESS_DETACH:

		// ���������� �����
		TRACE("DllMain: ���������� DLL\n");

		// ����������� TLS-index.
		TlsFree(gdwTlsIndex); 
		break;
	}

	return TRUE;
}
