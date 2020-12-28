/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * user32.dll functions.
  */

#pragma once

WRAPPER_DEC(LPCSTR WINAPI, CharNext,
	LPCSTR lpsz
);
#undef CharNext
#define CharNext CharNextU

WRAPPER_DEC(HWND WINAPI, CreateDialogParam,
	HINSTANCE hInstance,
	RESID lpTemplateRes,
	HWND hWndParent,
	DLGPROC lpDialogFunc,
	LPARAM dwInitParam
);
#undef CreateDialogParam
#define CreateDialogParam CreateDialogParamU

WRAPPER_DEC(HWND WINAPI, CreateWindowEx,
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
#undef CallWindowProc
#define CallWindowProc CallWindowProcW

WRAPPER_DEC(INT_PTR WINAPI, DialogBoxParam,
	HINSTANCE hInstance,
	RESID lpTemplateRes,
	HWND hWndParent,
	DLGPROC lpDialogFunc,
	LPARAM dwInitParam
);
#undef DialogBoxParam
#define DialogBoxParam DialogBoxParamU

WRAPPER_DEC(int WINAPI, DrawText,
	HDC hdc,
	LPCSTR lpchText,
	int cchText,
	LPRECT lprc,
	UINT format
);
#undef DrawText
#define DrawText DrawTextU

WRAPPER_DEC(BOOL WINAPI, GetClassInfo,
	HINSTANCE hInstance,
	LPCSTR lpClassName,
	LPWNDCLASSA lpWndClass
);
#undef GetClassInfo
#define GetClassInfo GetClassInfoU

WRAPPER_DEC(BOOL WINAPI, GetClassInfoEx,
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

WRAPPER_DEC(BOOL WINAPI, InsertMenuItem,
	HMENU hmenu,
	UINT item,
	BOOL fByPosition,
	LPCMENUITEMINFOA lpmi
);
#undef InsertMenuItem
#define InsertMenuItem InsertMenuItemU

WRAPPER_DEC(BOOL WINAPI, SetMenuItemInfo,
    HMENU hmenu,
    UINT item,
    BOOL fByPositon,
    LPCMENUITEMINFOA lpmi
);

#undef SetMenuItemInfo
#define SetMenuItemInfo SetMenuItemInfoU


WRAPPER_DEC(int WINAPI, LoadString,
	HINSTANCE hInstance,
	UINT uID,
	LPSTR lpBuffer,
	int cchBufferMax
);
#undef LoadString
#define LoadString LoadStringU

WRAPPER_DEC(int WINAPI, MessageBox,
	HWND hWnd,
	LPCSTR lpText,
	LPCSTR lpCaption,
	UINT uType
);
#undef MessageBox
#define MessageBox MessageBoxU

WRAPPER_DEC(int WINAPI, IsolationAwareMessageBox,
    HWND hWnd,
    LPCSTR lpText,
    LPCSTR lpCaption,
    UINT uType
);
#undef IsolationAwareMessageBox
#define IsolationAwareMessageBox IsolationAwareMessageBoxU


WRAPPER_DEC(ATOM WINAPI, RegisterClass,
	CONST WNDCLASSA *lpWndClass
);
#undef RegisterClass
#define RegisterClass RegisterClassU

WRAPPER_DEC(ATOM WINAPI, RegisterClassEx,
	CONST WNDCLASSEXA *lpWndClass
);
#undef RegisterClassEx
#define RegisterClassEx RegisterClassExU

WRAPPER_DEC(BOOL WINAPI, SetDlgItemText,
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

WRAPPER_DEC(BOOL WINAPI, SetWindowText,
	HWND hWnd,
	LPCSTR lpString
);
#undef SetWindowText
#define SetWindowText SetWindowTextU

WRAPPER_DEC(LONG WINAPI, TabbedTextOut,
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

WRAPPER_DEC(BOOL WINAPI, UnregisterClass,
	LPCSTR lpClassName,
	HINSTANCE hInstance
);
#undef UnregisterClass
#define UnregisterClass UnregisterClassU
