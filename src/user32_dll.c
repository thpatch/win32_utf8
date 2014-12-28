/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * user32.dll functions.
  */

#include "win32_utf8.h"

// Some of the message-related functions here link straight to their W
// counterparts. In these cases, both the A and W functions use the same
// parameters, but since we only ever create Unicode windows, we also have
// to ensure that Unicode is used on the entire message path.
const w32u8_pair_t user32_pairs[] = {
	{"CharNextA", CharNextU},
	{"CreateDialogParamA", CreateDialogParamU},
	{"CreateWindowExA", CreateWindowExU},
	{"DefWindowProcA", DefWindowProcW},
	{"DialogBoxParamA", DialogBoxParamU},
	{"DrawTextA", DrawTextU},
	{"GetClassInfoA", GetClassInfoU},
	{"GetClassInfoExA", GetClassInfoExU},
	{"GetWindowLongA", GetWindowLongW},
	{"GetWindowLongPtrA", GetWindowLongPtrW},
	{"InsertMenuItemA", InsertMenuItemU},
	{"LoadStringA", LoadStringU},
	{"MessageBoxA", MessageBoxU},
	{"RegisterClassA", RegisterClassU},
	{"RegisterClassExA", RegisterClassExU},
	{"SetDlgItemTextA", SetDlgItemTextU},
	{"SetWindowLongA", SetWindowLongW},
	{"SetWindowLongPtrA", SetWindowLongPtrW},
	{"SetWindowTextA", SetWindowTextU},
	{"TabbedTextOutA", TabbedTextOutU},
	{"UnregisterClassA", UnregisterClassU},
	NULL
};

/// Structure conversions
/// ---------------------
// Needs to be a macro because these members unfortunately have different
// offsets in WNDCLASS and WNDCLASSEX. Also, no reason to cast the individual
// W and A types to the common WNDCLASS(EX) just to calm down the compiler.
#define WndclassCopyBase(wc_dst, wc_src) \
	(wc_dst)->style = (wc_src)->style; \
	(wc_dst)->lpfnWndProc = (wc_src)->lpfnWndProc; \
	(wc_dst)->cbClsExtra = (wc_src)->cbClsExtra; \
	(wc_dst)->cbWndExtra = (wc_src)->cbWndExtra; \
	(wc_dst)->hInstance = (wc_src)->hInstance; \
	(wc_dst)->hIcon = (wc_src)->hIcon; \
	(wc_dst)->hCursor = (wc_src)->hCursor; \
	(wc_dst)->hbrBackground = (wc_src)->hbrBackground;

#define WndclassExCopyBase(wc_dst, wc_src) \
	(wc_dst)->cbSize = (wc_src)->cbSize; \
	(wc_dst)->hIconSm = (wc_src)->hIconSm;

#define WndclassAToW(w, a) \
	size_t lpszClassName_len = strlen((a)->lpszClassName) + 1; \
	VLA(wchar_t, lpszClassName_w, lpszClassName_len); \
	RESID_DEC(lpszMenuName); \
	(w)->lpszClassName = StringToUTF16_VLA(lpszClassName_w, (a)->lpszClassName, lpszClassName_len); \
	RESID_CONV(lpszMenuName, (a)->lpszMenuName); \
	(w)->lpszMenuName = lpszMenuName_w; \
	WndclassCopyBase((w), (a))

#define WndclassWClean(a) \
	VLA_FREE(lpszClassName_w); \
	RESID_FREE(lpszMenuName, (a)->lpszMenuName);
/// ---------------------

LPSTR WINAPI CharNextU(
	__in LPSTR lpsz
)
{
	LPSTR ret;
	extern UINT fallback_codepage;

	if(lpsz == NULL || *lpsz == '\0') {
		ret = lpsz;
	}
	else if(IsDBCSLeadByteEx(fallback_codepage, lpsz[0])) {
		int lpsz_len = strlen(lpsz);
		if(lpsz_len < 2) {
			ret = lpsz + 1;
		} else {
			ret = lpsz + 2;
		}
	}
	else {
		ret = lpsz + 1;
		if(!IsDBCSLeadByteEx(fallback_codepage, ret[0])) {
			// Get next UTF-8 char
			while((*ret & 0xc0) == 0x80) {
				++ret;
			}
		}
	}
	return ret;
}

HWND WINAPI CreateDialogParamU(
	__in_opt HINSTANCE hInstance,
	__in RESID lpTemplateRes,
	__in_opt HWND hWndParent,
	__in_opt DLGPROC lpDialogFunc,
	__in LPARAM dwInitParam
)
{
	HWND ret;
	RESID_DEC(lpTemplateRes);
	RESID_CONV(lpTemplateRes, lpTemplateRes);
	ret = CreateDialogParamW(
		hInstance, lpTemplateRes_w, hWndParent, lpDialogFunc, dwInitParam
	);
	RESID_FREE(lpTemplateRes, lpTemplateRes);
	return ret;
}

HWND WINAPI CreateWindowExU(
	__in DWORD dwExStyle,
	__in_opt LPCSTR lpClassName,
	__in_opt LPCSTR lpWindowName,
	__in DWORD dwStyle,
	__in int X,
	__in int Y,
	__in int nWidth,
	__in int nHeight,
	__in_opt HWND hWndParent,
	__in_opt HMENU hMenu,
	__in_opt HINSTANCE hInstance,
	__in_opt LPVOID lpParam
)
{
	HWND ret;
	WCHAR_T_DEC(lpClassName);
	WCHAR_T_DEC(lpWindowName);
	WCHAR_T_CONV_VLA(lpClassName);
	WCHAR_T_CONV_VLA(lpWindowName);

	ret = CreateWindowExW(
		dwExStyle, lpClassName_w, lpWindowName_w, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu,
		hInstance, lpParam
	);
	VLA_FREE(lpClassName_w);
	VLA_FREE(lpWindowName_w);
	return ret;
}

INT_PTR WINAPI DialogBoxParamU(
	__in_opt HINSTANCE hInstance,
	__in RESID lpTemplateRes,
	__in_opt HWND hWndParent,
	__in_opt DLGPROC lpDialogFunc,
	__in LPARAM dwInitParam
)
{
	INT_PTR ret;
	RESID_DEC(lpTemplateRes);
	RESID_CONV(lpTemplateRes, lpTemplateRes);
	ret = DialogBoxParamW(
		hInstance, lpTemplateRes_w, hWndParent, lpDialogFunc, dwInitParam
	);
	RESID_FREE(lpTemplateRes, lpTemplateRes);
	return ret;
}

int WINAPI DrawTextU(
	__in HDC hdc,
	__inout_ecount_opt(cchText) LPCSTR lpchText,
	__in int cchText,
	__inout LPRECT lprc,
	__in UINT format
)
{
	int ret;
	FixedLengthStringConvert(lpchText, cchText);
	ret = DrawTextW(hdc, lpchText_w, wcslen(lpchText_w) + 1, lprc, format);
	VLA_FREE(lpchText_w);
	return ret;
}

BOOL WINAPI GetClassInfoU(
	__in_opt HINSTANCE hInstance,
	__in LPCSTR lpClassName,
	__out LPWNDCLASSA wc
)
{
	WNDCLASSEXA wcex;
	BOOL ret = GetClassInfoExU(hInstance, lpClassName, &wcex);
	if(ret) {
		WndclassCopyBase(wc, &wcex);
		wc->lpszClassName = lpClassName;
	}
	return ret;
}

BOOL WINAPI GetClassInfoExU(
	__in_opt HINSTANCE hInstance,
	__in LPCSTR lpClassName,
	__out LPWNDCLASSEXA wcex_a
)
{
	BOOL ret;
	WNDCLASSEXW wcex_w = {sizeof(WNDCLASSEXW), 0};
	WCHAR_T_DEC(lpClassName);
	WCHAR_T_CONV_VLA(lpClassName);
	ret = GetClassInfoExW(hInstance, lpClassName_w, &wcex_w);
	if(ret) {
		WndclassCopyBase(wcex_a, &wcex_w);
		WndclassExCopyBase(wcex_a, &wcex_w);
		wcex_a->lpszClassName = lpClassName;
	}
	WCHAR_T_FREE(lpClassName);
	return ret;
}

BOOL WINAPI InsertMenuItemU(
	__in HMENU hmenu,
	__in UINT item,
	__in BOOL fByPosition,
	__in LPCMENUITEMINFOA lpmi
)
{
	BOOL ret;
	MENUITEMINFOW lpmi_w;
	wchar_t *str_w = NULL;
	if(lpmi) {
		memcpy(&lpmi_w, lpmi, sizeof(MENUITEMINFOW));
		if(lpmi->fMask & MIIM_TYPE || lpmi->fMask & MIIM_STRING) {
			// yes, [cch] is ignored
			const char *str_local = lpmi->dwTypeData;
			WCHAR_T_DEC(str_local);
			WCHAR_T_CONV(str_local);
			str_w = lpmi_w.dwTypeData = str_local_w;
		}
	} else {
		ZeroMemory(&lpmi_w, sizeof(MENUITEMINFOW));
	}
	ret = InsertMenuItemW(hmenu, item, fByPosition, &lpmi_w);
	VLA_FREE(str_w);
	return ret;
}

int WINAPI LoadStringU(
	__in_opt HINSTANCE hInstance,
	__in UINT uID,
	__out_ecount_part(cchBufferMax, return + 1) LPSTR lpBuffer,
	__in int cchBufferMax
)
{
	int ret = -1;
	// Since LoadStringW() could also return double-null-terminated strings
	// (see http://blogs.msdn.com/b/oldnewthing/archive/2009/10/09/9904648.aspx),
	// this indeed is not as easy as simply calling LoadStringW() and
	// converting the result.
	HRSRC hrsrc = FindResource(hInstance,
		MAKEINTRESOURCEW((LOWORD(uID) >> 4) + 1), (LPWSTR)RT_STRING
	);
	IMAGE_RESOURCE_DIR_STRING_U *str_res = LoadResource(hInstance, hrsrc);
	if(hrsrc && str_res && cchBufferMax) {
		unsigned int id = uID & 0x000f;
		while(id--) {
			str_res = (IMAGE_RESOURCE_DIR_STRING_U*)(
				((LPCWSTR)str_res) + str_res->Length + 1
			);
		}
		ret = StringToMBFixed(
			lpBuffer, str_res->NameString, cchBufferMax, str_res->Length
		);
		lpBuffer[ret] = 0;
	}
	return ret;
}

int WINAPI MessageBoxU(
	__in_opt HWND hWnd,
	__in_opt LPCSTR lpText,
	__in_opt LPCSTR lpCaption,
	__in UINT uType
)
{
	int ret;
	WCHAR_T_DEC(lpText);
	WCHAR_T_DEC(lpCaption);
	WCHAR_T_CONV_VLA(lpText);
	WCHAR_T_CONV_VLA(lpCaption);
	ret = MessageBoxW(hWnd, lpText_w, lpCaption_w, uType);
	VLA_FREE(lpText_w);
	VLA_FREE(lpCaption_w);
	return ret;
}

ATOM WINAPI RegisterClassU(
	__in CONST WNDCLASSA *lpWndClass
)
{
	ATOM ret;
	WNDCLASSW WndClassW;
	WndclassAToW(&WndClassW, lpWndClass);
	ret = RegisterClassW(&WndClassW);
	WndclassWClean(lpWndClass);
	return ret;
}

ATOM WINAPI RegisterClassExU(
	__in CONST WNDCLASSEXA *lpWndClass
)
{
	ATOM ret;
	WNDCLASSEXW WndClassW;
	WndclassAToW(&WndClassW, lpWndClass);
	WndclassExCopyBase(&WndClassW, lpWndClass);
	ret = RegisterClassExW(&WndClassW);
	WndclassWClean(lpWndClass);
	return ret;
}

BOOL WINAPI SetDlgItemTextU(
	__in HWND hDlg,
	__in int nIDDlgItem,
	__in LPCSTR lpString
)
{
	BOOL ret;
	WCHAR_T_DEC(lpString);
	WCHAR_T_CONV_VLA(lpString);
	ret = SetDlgItemTextW(hDlg, nIDDlgItem, lpString_w);
	WCHAR_T_FREE(lpString);
	return ret;
}

BOOL WINAPI SetWindowTextU(
	__in HWND hWnd,
	__in_opt LPCSTR lpString
)
{
	BOOL ret;
	WCHAR_T_DEC(lpString);
	WCHAR_T_CONV_VLA(lpString);
	ret = SetWindowTextW(hWnd, lpString_w);
	VLA_FREE(lpString_w);
	return ret;
}

LONG WINAPI TabbedTextOutU(
	__in HDC hdc,
	__in int x,
	__in int y,
	__in_ecount(chCount) LPCSTR lpString,
	__in int chCount,
	__in int nTabPositions,
	__in_ecount_opt(nTabPositions) CONST INT *lpnTabStopPositions,
	__in int nTabOrigin
)
{
	BOOL ret;
	FixedLengthStringConvert(lpString, chCount);
	ret = TabbedTextOutW(
		hdc, x, y, lpString_w, wcslen(lpString_w),
		nTabPositions, lpnTabStopPositions, nTabOrigin
	);
	WCHAR_T_FREE(lpString);
	return ret;
}

BOOL WINAPI UnregisterClassU(
	__in LPCSTR lpClassName,
	__in_opt HINSTANCE hInstance
)
{
	BOOL ret;
	WCHAR_T_DEC(lpClassName);
	WCHAR_T_CONV_VLA(lpClassName);
	ret = UnregisterClassW(lpClassName_w, hInstance);
	WCHAR_T_FREE(lpClassName);
	return ret;
}
