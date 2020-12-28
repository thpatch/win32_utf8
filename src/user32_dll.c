/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * user32.dll functions.
  */

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
    {"SetMenuItemInfoA", SetMenuItemInfoU},
	{"LoadStringA", LoadStringU},
	{"MessageBoxA", MessageBoxU},
    {"IsolationAwareMessageBoxA", IsolationAwareMessageBoxU},
	{"RegisterClassA", RegisterClassU},
	{"RegisterClassExA", RegisterClassExU},
	{"SetDlgItemTextA", SetDlgItemTextU},
	{"SetWindowLongA", SetWindowLongW},
	{"SetWindowLongPtrA", SetWindowLongPtrW},
	{"SetWindowTextA", SetWindowTextU},
	{"TabbedTextOutA", TabbedTextOutU},
	{"UnregisterClassA", UnregisterClassU},
	{ NULL }
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

LPCSTR WINAPI CharNextU(
	LPCSTR lpsz
)
{
	LPCSTR ret;
	extern UINT fallback_codepage;

	if(lpsz == NULL || *lpsz == '\0') {
		ret = lpsz;
	}
	if ((lpsz[0] & 0xe0) == 0xc0 && (lpsz[1] & 0xc0) == 0x80) {
		ret = lpsz + 2;
	}
	else if ((lpsz[0] & 0xf0) == 0xe0 && (lpsz[1] & 0xc0) == 0x80 && (lpsz[2] & 0xc0) == 0x80) {
		ret = lpsz + 3;
	}
	else if ((lpsz[0] & 0xf8) == 0xf0 && (lpsz[1] & 0xc0) == 0x80 && (lpsz[2] & 0xc0) == 0x80 && (lpsz[3] & 0xc0) == 0x80) {
		ret = lpsz + 4;
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
	}
	return ret;
}

HWND WINAPI CreateDialogParamU(
	HINSTANCE hInstance,
	RESID lpTemplateRes,
	HWND hWndParent,
	DLGPROC lpDialogFunc,
	LPARAM dwInitParam
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
)
{
	HWND ret;
	RESID_DEC(lpClassName);
	WCHAR_T_DEC(lpWindowName);
	RESID_CONV(lpClassName, lpClassName);
	WCHAR_T_CONV(lpWindowName);

	ret = CreateWindowExW(
		dwExStyle, lpClassName_w, lpWindowName_w, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu,
		hInstance, lpParam
	);
	RESID_FREE(lpClassName, lpClassName);
	WCHAR_T_FREE(lpWindowName);
	return ret;
}

INT_PTR WINAPI DialogBoxParamU(
	HINSTANCE hInstance,
	RESID lpTemplateRes,
	HWND hWndParent,
	DLGPROC lpDialogFunc,
	LPARAM dwInitParam
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
	HDC hdc,
	LPCSTR lpchText,
	int cchText,
	LPRECT lprc,
	UINT format
)
{
	int ret;
	FixedLengthStringConvert(lpchText, cchText);
	ret = DrawTextW(hdc, lpchText_w, lpchText_w_len, lprc, format);
	WCHAR_T_FREE(lpchText);
	return ret;
}

BOOL WINAPI GetClassInfoU(
	HINSTANCE hInstance,
	LPCSTR lpClassName,
	LPWNDCLASSA wc
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
	HINSTANCE hInstance,
	LPCSTR lpClassName,
	LPWNDCLASSEXA wcex_a
)
{
	BOOL ret;
	WNDCLASSEXW wcex_w = {0};
	wcex_w.cbSize = sizeof(WNDCLASSEXW);
	RESID_DEC(lpClassName);
	RESID_CONV(lpClassName, lpClassName);
	ret = GetClassInfoExW(hInstance, lpClassName_w, &wcex_w);
	if(ret) {
		WndclassCopyBase(wcex_a, &wcex_w);
		WndclassExCopyBase(wcex_a, &wcex_w);
		wcex_a->lpszClassName = lpClassName;
	}
	RESID_FREE(lpClassName, lpClassName);
	return ret;
}

BOOL WINAPI InsertMenuItemU(
	HMENU hmenu,
	UINT item,
	BOOL fByPosition,
	LPCMENUITEMINFOA lpmi
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

BOOL WINAPI SetMenuItemInfoU(
    HMENU hmenu,
    UINT item,
    BOOL fByPosition,
    LPCMENUITEMINFOA lpmi
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
    ret = SetMenuItemInfoW(hmenu, item, fByPosition, &lpmi_w);
    VLA_FREE(str_w);
    return ret;
}

int WINAPI LoadStringU(
	HINSTANCE hInstance,
	UINT uID,
	LPSTR lpBuffer,
	int cchBufferMax
)
{
	int ret = -1;
	// Since LoadStringW() could also return double-null-terminated strings
	// (see http://blogs.msdn.com/b/oldnewthing/archive/2009/10/09/9904648.aspx),
	// this indeed is not as easy as simply calling LoadStringW() and
	// converting the result.
	HRSRC hrsrc = FindResourceW(hInstance,
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
	HWND hWnd,
	LPCSTR lpText,
	LPCSTR lpCaption,
	UINT uType
)
{
	int ret;
	WCHAR_T_DEC(lpText);
	WCHAR_T_DEC(lpCaption);
	WCHAR_T_CONV(lpText);
	WCHAR_T_CONV(lpCaption);
	ret = MessageBoxW(hWnd, lpText_w, lpCaption_w, uType);
	WCHAR_T_FREE(lpText);
	WCHAR_T_FREE(lpCaption);
	return ret;
}

int WINAPI IsolationAwareMessageBoxU(
    HWND hWnd,
    LPCSTR lpText,
    LPCSTR lpCaption,
    UINT uType
)
{
    int ret;
    WCHAR_T_DEC(lpText);
    WCHAR_T_DEC(lpCaption);
    WCHAR_T_CONV(lpText);
    WCHAR_T_CONV(lpCaption);
    ret = IsolationAwareMessageBoxW(hWnd, lpText_w, lpCaption_w, uType);
    WCHAR_T_FREE(lpText);
    WCHAR_T_FREE(lpCaption);
    return ret;
}

ATOM WINAPI RegisterClassU(
	CONST WNDCLASSA *lpWndClass
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
	CONST WNDCLASSEXA *lpWndClass
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
	HWND hDlg,
	int nIDDlgItem,
	LPCSTR lpString
)
{
	BOOL ret;
	WCHAR_T_DEC(lpString);
	WCHAR_T_CONV(lpString);
	ret = SetDlgItemTextW(hDlg, nIDDlgItem, lpString_w);
	WCHAR_T_FREE(lpString);
	return ret;
}

BOOL WINAPI SetWindowTextU(
	HWND hWnd,
	LPCSTR lpString
)
{
	BOOL ret;
	WCHAR_T_DEC(lpString);
	WCHAR_T_CONV(lpString);
	ret = SetWindowTextW(hWnd, lpString_w);
	WCHAR_T_FREE(lpString);
	return ret;
}

LONG WINAPI TabbedTextOutU(
	HDC hdc,
	int x,
	int y,
	LPCSTR lpString,
	int chCount,
	int nTabPositions,
	CONST INT *lpnTabStopPositions,
	int nTabOrigin
)
{
	BOOL ret;
	FixedLengthStringConvert(lpString, chCount);
	ret = TabbedTextOutW(
		hdc, x, y, lpString_w, lpString_w_len,
		nTabPositions, lpnTabStopPositions, nTabOrigin
	);
	WCHAR_T_FREE(lpString);
	return ret;
}

BOOL WINAPI UnregisterClassU(
	LPCSTR lpClassName,
	HINSTANCE hInstance
)
{
	BOOL ret;
	RESID_DEC(lpClassName);
	RESID_CONV(lpClassName, lpClassName);
	ret = UnregisterClassW(lpClassName_w, hInstance);
	RESID_FREE(lpClassName, lpClassName);
	return ret;
}
