//////////////////////////////////////////////////////////////////////
// MyLib.cpp - исходный файл DLL-модуля

#include <stdio.h>
#include "MyLib.h"

int count = 0;
//__declspec(thread) int count = 0;

EXPORT void Print(LPCTSTR text)
{
	count++;
	char title[40];
	sprintf(title, "MyLib: Print: вызов %d", count);
	MessageBox(NULL, text, title, MB_OK);
}
