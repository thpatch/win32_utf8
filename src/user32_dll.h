/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * user32.dll functions.
  */

#pragma once

LPSTR WINAPI CharNextU(
	LPSTR lpsz
);
#undef CharNext
#define CharNext CharNextU

HWND WINAPI CreateDialogParamU(
	HINSTANCE hInstance,
	RESID lpTemplateRes,
	HWND hWndParent,
	DLGPROC lpDialogFunc,
	LPARAM dwInitParam
);
#undef CreateDialogParam
#define CreateDialogParam CreateDialogParamU

HWND WINAPI CreateWindowExU(
	DWORD dwExStyle,
	LPCSTR lpClassName,
	LPCSTR lpWindowName,
	DWORD dwStyle,
	int X,
	int Y,
	int nWidth,
	int nHeight,
	HWND hWndParent,
	HMENU hMenu,
	HINSTANCE hInstance,
	LPVOID lpParam
);
#undef CreateWindowEx
#define CreateWindowEx CreateWindowExU

// Yep, both original functions use the same parameters
#undef DefWindowProc
#define DefWindowProc DefWindowProcW

INT_PTR WINAPI DialogBoxParamU(
	HINSTANCE hInstance,
	RESID lpTemplateRes,
	HWND hWndParent,
	DLGPROC lpDialogFunc,
	LPARAM dwInitParam
);
#undef DialogBoxParam
#define DialogBoxParam DialogBoxParamU

int WINAPI DrawTextU(
	HDC hdc,
	LPCSTR lpchText,
	int cchText,
	LPRECT lprc,
	UINT format
);
#undef DrawText
#define DrawText DrawTextU

BOOL WINAPI GetClassInfoU(
	HINSTANCE hInstance,
	LPCSTR lpClassName,
	LPWNDCLASSA lpWndClass
);
#undef GetClassInfo
#define GetClassInfo GetClassInfoU

BOOL WINAPI GetClassInfoExU(
	HINSTANCE hInstance,
	LPCSTR lpszClass,
	LPWNDCLASSEXA lpwcx
);
#undef GetClassInfoEx
#define GetClassInfoEx GetClassInfoExU

// These (and SetWindowLong(Ptr) below) are necessary because Windows otherwise
//  silently converts certain text parameters for window procedures to ANSI.
// (see http://blogs.msdn.com/b/oldnewthing/archive/2003/12/01/55900.aspx)
#undef GetWindowLong
#undef GetWindowLongPtr
#define GetWindowLong GetWindowLongW
#define GetWindowLongPtr GetWindowLongPtrW

BOOL WINAPI InsertMenuItemU(
	HMENU hmenu,
	UINT item,
	BOOL fByPosition,
	LPCMENUITEMINFOA lpmi
);
#undef InsertMenuItem
#define InsertMenuItem InsertMenuItemU

int WINAPI LoadStringU(
	HINSTANCE hInstance,
	UINT uID,
	LPSTR lpBuffer,
	int cchBufferMax
);
#undef LoadString
#define LoadString LoadStringU

int WINAPI MessageBoxU(
	HWND hWnd,
	LPCSTR lpText,
	LPCSTR lpCaption,
	UINT uType
);
#undef MessageBox
#define MessageBox MessageBoxU

ATOM WINAPI RegisterClassU(
	CONST WNDCLASSA *lpWndClass
);
#undef RegisterClass
#define RegisterClass RegisterClassU

ATOM WINAPI RegisterClassExU(
	CONST WNDCLASSEXA *lpWndClass
);
#undef RegisterClassEx
#define RegisterClassEx RegisterClassExU

BOOL WINAPI SetDlgItemTextU(
	HWND hDlg,
	int nIDDlgItem,
	LPCSTR lpString
);
#undef SetDlgItemText
#define SetDlgItemText SetDlgItemTextU

#undef SetWindowLong
#undef SetWindowLongPtr
#define SetWindowLong SetWindowLongW
#define SetWindowLongPtr SetWindowLongPtrW

BOOL WINAPI SetWindowTextU(
	HWND hWnd,
	LPCSTR lpString
);
#undef SetWindowText
#define SetWindowText SetWindowTextU

LONG WINAPI TabbedTextOutU(
	HDC hdc,
	int x,
	int y,
	LPCSTR lpString,
	int chCount,
	int nTabPositions,
	CONST INT *lpnTabStopPositions,
	int nTabOrigin
);
#undef TabbedTextOut
#define TabbedTextOut TabbedTextOutU

BOOL WINAPI UnregisterClassU(
	LPCSTR lpClassName,
	HINSTANCE hInstance
);
#undef UnregisterClass
#define UnregisterClass UnregisterClassU
