/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * version.dll functions.
  */

#include "win32_utf8.h"

/// Functions not available before Vista
/// ------------------------------------
typedef DWORD WINAPI DLL_FUNC_TYPE(version, GetFileVersionInfoSizeExW)(
	__in DWORD dwFlags,
	__in LPCWSTR lpstrFilename,
	__out LPDWORD lpdwHandle
);

typedef BOOL WINAPI DLL_FUNC_TYPE(version, GetFileVersionInfoExW)(
	__in DWORD dwFlags,
	__in LPCWSTR lpstrFilename,
	__reserved DWORD dwHandle,
	__in DWORD dwLen,
	__out_bcount(dwLen) LPVOID lpData
);

DLL_FUNC_DEF(version, GetFileVersionInfoSizeExW);
DLL_FUNC_DEF(version, GetFileVersionInfoExW);
/// ------------------------------------

const w32u8_pair_t version_pairs[] = {
	{"GetFileVersionInfoA", GetFileVersionInfoU},
	{"GetFileVersionInfoExA", GetFileVersionInfoExU},
	{"GetFileVersionInfoSizeA", GetFileVersionInfoSizeU},
	{"GetFileVersionInfoSizeExA", GetFileVersionInfoSizeExU},
	NULL
};

BOOL WINAPI GetFileVersionInfoU(
	__in LPCSTR lpstrFilename,
	__reserved DWORD dwHandle,
	__in DWORD dwLen,
	__out_bcount(dwLen) LPVOID lpData
)
{
	BOOL ret;
	WCHAR_T_DEC(lpstrFilename);
	WCHAR_T_CONV_VLA(lpstrFilename);
	ret = GetFileVersionInfoW(lpstrFilename_w, dwHandle, dwLen, lpData);
	WCHAR_T_FREE(lpstrFilename);
	return ret;
}

BOOL WINAPI GetFileVersionInfoExU(
	__in DWORD dwFlags,
	__in LPCSTR lpstrFilename,
	__reserved DWORD dwHandle,
	__in DWORD dwLen,
	__out_bcount(dwLen) LPVOID lpData
)
{
	BOOL ret;
	WCHAR_T_DEC(lpstrFilename);
	WCHAR_T_CONV_VLA(lpstrFilename);
	DLL_FUNC_CALL(version, GetFileVersionInfoExW,
		dwFlags, lpstrFilename_w, dwHandle, dwLen, lpData
	);
	WCHAR_T_FREE(lpstrFilename);
	return ret;
}

DWORD WINAPI GetFileVersionInfoSizeU(
	__in LPCSTR lpstrFilename,
	__out_opt LPDWORD lpdwHandle
)
{
	BOOL ret;
	WCHAR_T_DEC(lpstrFilename);
	WCHAR_T_CONV_VLA(lpstrFilename);
	ret = GetFileVersionInfoSizeW(lpstrFilename_w, lpdwHandle);
	WCHAR_T_FREE(lpstrFilename);
	return ret;
}

DWORD WINAPI GetFileVersionInfoSizeExU(
	__in DWORD dwFlags,
	__in LPCSTR lpstrFilename,
	__out LPDWORD lpdwHandle
)
{
	DWORD ret;
	WCHAR_T_DEC(lpstrFilename);
	WCHAR_T_CONV_VLA(lpstrFilename);
	DLL_FUNC_CALL(version, GetFileVersionInfoSizeExW,
		dwFlags, lpstrFilename_w, lpdwHandle
	);
	WCHAR_T_FREE(lpstrFilename);
	return ret;
}

void version_init(void)
{
	HMODULE version = GetModuleHandleA("version.dll");
	if(version) {
		DLL_FUNC_GET(version, GetFileVersionInfoSizeExW);
		DLL_FUNC_GET(version, GetFileVersionInfoExW);
	}
}
