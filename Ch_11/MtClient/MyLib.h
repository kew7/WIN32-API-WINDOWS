//////////////////////////////////////////////////////////////////////
// MyLib.h - ������������ ���� DLL-������

#ifndef _MYLIB_H 
#define _MYLIB_H


#include <windows.h>
 
#define EXPORT extern "C" __declspec(dllexport)

// ����� ������������ ��������� �������������� �������
EXPORT void Print(LPCTSTR text);

#endif   // _MYLIB_H

//////////////////////////////////////////////////////////////////////
