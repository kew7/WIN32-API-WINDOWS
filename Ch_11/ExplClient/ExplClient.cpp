//////////////////////////////////////////////////////////////////////
// ExplClient.cpp
#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	// Дескриптор загружаемой DLL
	HMODULE hLib;

	// Загружаем библиотеку
	hLib = LoadLibrary("MyLib.dll");

	if (!hLib) {
		MessageBox(NULL, "Библиотека MyLib.dll не найдена", "Error", MB_OK);
		return 1;
	}

	// MYPROC - тип указателя на функцию, совместимый с прототипом функции,
	//  вызываемой из DLL
	typedef void (*MYPROC) (LPCTSTR str);

	// Объявляем указатель на функцию типа MYPROC
	MYPROC pMyProc;

	// Вызовом GetProcAddress получаем адрес функции Print
	pMyProc = (MYPROC)(GetProcAddress(hLib,"Print"));

	if (!pMyProc) {
		MessageBox(NULL, "В DLL отсутствует функия Print", "Error", MB_OK);
		return 1;
	}

	// Используем функцию Print из DLL
	pMyProc("Hello!");
	pMyProc("The experiment was a success!");
	pMyProc("By!");

	// Выгружаем библиотеку из памяти
	FreeLibrary(hLib);

	return 0;
}
