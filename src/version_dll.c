/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * version.dll functions.
  */

#include "win32_utf8.h"

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
	return GetFileVersionInfoExU(
		FILE_VER_GET_LOCALISED | FILE_VER_GET_NEUTRAL,
		lpstrFilename, dwHandle, dwLen, lpData
	);
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
	ret = GetFileVersionInfoExW(
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
	return GetFileVersionInfoSizeExU(
		FILE_VER_GET_LOCALISED, lpstrFilename, lpdwHandle
	);
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
	ret = GetFileVersionInfoSizeExW(dwFlags, lpstrFilename_w, lpdwHandle);
	WCHAR_T_FREE(lpstrFilename);
	return ret;
}
