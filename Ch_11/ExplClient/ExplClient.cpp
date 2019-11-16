//////////////////////////////////////////////////////////////////////
// ExplClient.cpp
#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	// ���������� ����������� DLL
	HMODULE hLib;

	// ��������� ����������
	hLib = LoadLibrary("MyLib.dll");

	if (!hLib) {
		MessageBox(NULL, "���������� MyLib.dll �� �������", "Error", MB_OK);
		return 1;
	}

	// MYPROC - ��� ��������� �� �������, ����������� � ���������� �������,
	//  ���������� �� DLL
	typedef void (*MYPROC) (LPCTSTR str);

	// ��������� ��������� �� ������� ���� MYPROC
	MYPROC pMyProc;

	// ������� GetProcAddress �������� ����� ������� Print
	pMyProc = (MYPROC)(GetProcAddress(hLib,"Print"));

	if (!pMyProc) {
		MessageBox(NULL, "� DLL ����������� ������ Print", "Error", MB_OK);
		return 1;
	}

	// ���������� ������� Print �� DLL
	pMyProc("Hello!");
	pMyProc("The experiment was a success!");
	pMyProc("By!");

	// ��������� ���������� �� ������
	FreeLibrary(hLib);

	return 0;
}
