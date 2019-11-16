//////////////////////////////////////////////////////////////////////
// MyNotebook1.cpp
#include <windows.h>
#include <fstream>
using namespace std;

#include "KWnd.h"
#include "resource.h"

#define N1  21
#define N2  81
#define TOTAL_SIZE (3*N1 + N2)

fstream fdat;

struct SubStr {
	char name[N1];
	char phone[N1];
	char birthday[N1];
	char address[N2];
};
union Data {
	SubStr ss;
	char line[TOTAL_SIZE];
};

class ListItem {
	char buf[TOTAL_SIZE];
public:
	ListItem() { Clear(); }
	
	// Очистка данных (заполнение строки line пробелами)
	void Clear() { memset(dat.line, ' ', TOTAL_SIZE); }
	
	// Замена в строке line всех нулевых байтов, кроме последнего,
	//     символом 'пробел'
	void DoOneString() {
		int count = 0;
		for (int i = TOTAL_SIZE - 1; i > 0; --i)
			if (!dat.line[i])
				if (!count)  count++;
				else  dat.line[i] = ' ';
	}
	
	const char* GetBirthday() {
		memcpy(buf, dat.ss.birthday, N1);
		buf[N1-1] = 0;
		return buf;
	}
	
	const char* GetAddress() {
		memcpy(buf, dat.ss.address, N2);
		buf[N2-1] = 0;
		return buf;
	}

	Data dat;	
};

ListItem item;

BOOL CALLBACK ViewDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK AddRecDiaProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//==================================================================== 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	KWnd mainWnd("MyNotebook1", hInstance, nCmdShow, WndProc,
		MAKEINTRESOURCE(IDR_MENU1), 100, 100, 400, 300);	

	while (GetMessage(&msg, NULL, 0, 0))  {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

//==================================================================== 
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HINSTANCE hInst;

	switch (uMsg) {
	
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDM_VIEW:
			hInst = GetModuleHandle(NULL);
			DialogBox(hInst, MAKEINTRESOURCE(IDD_VIEW), hWnd, ViewDlgProc);  
			break;
		}
		break;

    case WM_DESTROY:
		PostQuitMessage(0);  break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

//==================================================================== 
BOOL CALLBACK ViewDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	static HWND hListBox;
	static HWND hEditInfoAddr, hEditInfoBday;
	HWND hCtrl;
	static LOGFONT lf;
	HFONT hFont1;
	COLORREF editInfoColor;
	static HBRUSH hBrush;
	int nItems;   // количество элементов в списке
	int iCurItem; // текущий выбранный элемент списка
	BOOL dlgYes;
	int i;

	switch (uMsg) {
	case WM_INITDIALOG:
		hListBox = GetDlgItem(hDlg, IDC_LIST1);
		hEditInfoAddr = GetDlgItem(hDlg, IDC_INFO_ADDR);
		hEditInfoBday = GetDlgItem(hDlg, IDC_INFO_BDAY);

		// Модификация шрифта для элемента hListBox
		lf.lfHeight = 16;
		lstrcpy( (LPSTR)&lf.lfFaceName, "Courier" );
		hFont1 = CreateFontIndirect(&lf);
		SendMessage(hListBox, WM_SETFONT, (WPARAM)hFont1, TRUE );
		// Создание кисти для фона элементов управления
		editInfoColor = RGB(190, 255, 255);
		hBrush = CreateSolidBrush(editInfoColor);

		// Чтение "базы данных" из файла и инициализация списка
		fdat.open("notebook.dat", ios::in);
		if (!fdat) { fdat.clear();  return TRUE; }
		else while (1) {
			fdat.getline(item.dat.line, sizeof(Data));
			if (fdat.eof()) { fdat.clear();  break; }
			SendMessage(hListBox, LB_ADDSTRING, 0,
				(LPARAM)item.dat.line);			
		}
		fdat.close();
		return TRUE;

	case WM_CTLCOLORSTATIC:
		// Модификация цветовых атрибутов окон редактирования
		hCtrl = (HWND)lParam;
		if ((hCtrl == hEditInfoAddr) || (hCtrl == hEditInfoBday)) {
			SetBkColor((HDC)wParam, editInfoColor);
			SetBkMode((HDC)wParam, TRANSPARENT);
			return (DWORD)hBrush;
		}
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_REC_ADD:
			dlgYes = DialogBox((HINSTANCE)GetModuleHandle(NULL),
			   MAKEINTRESOURCE(IDD_ADD_REC), hDlg, AddRecDiaProc);
			if (dlgYes)
				SendMessage(hListBox, LB_ADDSTRING, 0,
							(LPARAM)item.dat.line);			
			break;

		case IDC_REC_DELETE:
			iCurItem = SendMessage(hListBox, LB_GETCURSEL, 0, 0);
			if (iCurItem != LB_ERR) {
				SendMessage(hListBox, LB_DELETESTRING, iCurItem, 0);
				SetDlgItemText(hDlg, IDC_INFO_ADDR, ""); 
				SetDlgItemText(hDlg, IDC_INFO_BDAY, ""); 
			}
			else {
				MessageBox(hDlg, "Сначала надо выделить элемент списка", 
				           "Ошибка", MB_OK);
				break;
			}
			break;

		case IDC_LIST1: 
			switch (HIWORD(wParam)) { 
			case LBN_SELCHANGE: 
				// Показываем дополнительную информацию
				iCurItem = SendMessage(hListBox, LB_GETCURSEL, 0, 0);
				SendMessage(hListBox, LB_GETTEXT, iCurItem,
					(LPARAM)item.dat.line);
				SetDlgItemText(hDlg, IDC_INFO_ADDR, item.GetAddress()); 
				SetDlgItemText(hDlg, IDC_INFO_BDAY, item.GetBirthday()); 
                return TRUE; 
			} 
			break; 

		case IDOK:			
			fdat.open("notebook.dat", ios::out | ios::trunc);
			if (!fdat) {
				MessageBox(hDlg, "Не могу открыть файл 'notebook.dat' для записи!",
					"Ошибка", MB_OK);
				return FALSE;
			}
			nItems = SendMessage(hListBox, LB_GETCOUNT, 0, 0);						
			for (i = 0; i < nItems; ++i) {
				SendMessage(hListBox, LB_GETTEXT, i, (LPARAM)item.dat.line);
				fdat << item.dat.line << endl;
			}
			fdat.close();
			
			EndDialog(hDlg, TRUE);
			return TRUE;
		
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			return TRUE;		
		}
		break;
	}
	return FALSE;
}

//==================================================================== 
BOOL CALLBACK AddRecDiaProc(HWND hDlg, UINT uMsg, 
			 WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {

	case WM_INITDIALOG:
		SetFocus(GetDlgItem(hDlg, IDC_INPUT_NAME));
		return FALSE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			item.Clear();
			GetDlgItemText(hDlg, IDC_INPUT_NAME, item.dat.ss.name, N1);
			GetDlgItemText(hDlg, IDC_INPUT_PHONE, item.dat.ss.phone, N1);
			GetDlgItemText(hDlg, IDC_INPUT_BDAY, item.dat.ss.birthday, N1);
			GetDlgItemText(hDlg, IDC_INPUT_ADDR, item.dat.ss.address, N2);
			item.DoOneString();
			EndDialog(hDlg, TRUE);
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			return TRUE;		
		}
		break;	
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
