//////////////////////////////////////////////////////////////////////
// MyLib.h - заголовочный файл DLL-модул€

#ifndef _MYLIB_H 
#define _MYLIB_H


#include <windows.h>
 
#define EXPORT extern "C" __declspec(dllexport)

// «десь определ€ютс€ прототипы экспортируемых функций
EXPORT void Print(LPCTSTR text);

#endif   // _MYLIB_H

//////////////////////////////////////////////////////////////////////
