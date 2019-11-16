//////////////////////////////////////////////////////////////////////
// ClientDllRes.cpp
//--------------------------------------------------------------------
//  ������ ��������� �� ��������� ������ � ����� ����������:
//     ClientDllRes.exe Eng    - ��� ������������� ������������
//     ClientDllRes.exe Rus    - ��� �������������� ������������
//--------------------------------------------------------------------
#include <windows.h>
#include "MyLib.h"
#include "MyResEng.h"
#include "MyResRus.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	
	char cmdLine[100];
	strcpy(cmdLine, lpCmdLine);

	HMODULE hLib;
	if (!strcmp(cmdLine, "Eng"))
		hLib = LoadLibraryEx("MyResEng.dll", 0, LOAD_LIBRARY_AS_DATAFILE);
	else if (!strcmp(cmdLine, "Rus"))
		hLib = LoadLibraryEx("MyResRus.dll", 0, LOAD_LIBRARY_AS_DATAFILE);
	else {
		MessageBox(NULL, "����������� ��� ������� ����� �������� � ��������� ������",
			"Error", MB_OK);
		return 0;
	}

	char buf[100];
	LoadString(hLib, IDS_1, buf, 99);
	Print(buf);

	LoadString(hLib, IDS_2, buf, 99);
	Print(buf);

	LoadString(hLib, IDS_3, buf, 99);
	Print(buf);

	return 0;
}
