/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * version.dll functions.
  */

#pragma once

BOOL WINAPI GetFileVersionInfoU(
	__in LPCSTR lpstrFilename,
	__reserved DWORD dwHandle,
	__in DWORD dwLen,
	__out_bcount(dwLen) LPVOID lpData
);
#undef GetFileVersionInfo
#define GetFileVersionInfo GetFileVersionInfoU

BOOL WINAPI GetFileVersionInfoExU(
	__in DWORD dwFlags,
	__in LPCSTR lpstrFilename,
	__reserved DWORD dwHandle,
	__in DWORD dwLen,
	__out_bcount(dwLen) LPVOID lpData
);
#undef GetFileVersionInfoEx
#define GetFileVersionInfoEx GetFileVersionInfoExU

DWORD WINAPI GetFileVersionInfoSizeU(
	__in LPCSTR lpstrFilename,
	__out_opt LPDWORD lpdwHandle
);
#undef GetFileVersionInfoSize
#define GetFileVersionInfoSize GetFileVersionInfoSizeU

DWORD WINAPI GetFileVersionInfoSizeExU(
	__in DWORD dwFlags,
	__in LPCSTR lpstrFilename,
	__out LPDWORD lpdwHandle
);
#undef GetFileVersionInfoSizeEx
#define GetFileVersionInfoSizeEx GetFileVersionInfoSizeExU

void version_init(void);
