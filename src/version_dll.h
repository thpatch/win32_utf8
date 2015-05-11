/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * version.dll functions.
  */

#pragma once

BOOL WINAPI GetFileVersionInfoU(
	LPCSTR lpstrFilename,
	DWORD dwHandle,
	DWORD dwLen,
	LPVOID lpData
);
#undef GetFileVersionInfo
#define GetFileVersionInfo GetFileVersionInfoU

BOOL WINAPI GetFileVersionInfoExU(
	DWORD dwFlags,
	LPCSTR lpstrFilename,
	DWORD dwHandle,
	DWORD dwLen,
	LPVOID lpData
);
#undef GetFileVersionInfoEx
#define GetFileVersionInfoEx GetFileVersionInfoExU

DWORD WINAPI GetFileVersionInfoSizeU(
	LPCSTR lpstrFilename,
	LPDWORD lpdwHandle
);
#undef GetFileVersionInfoSize
#define GetFileVersionInfoSize GetFileVersionInfoSizeU

DWORD WINAPI GetFileVersionInfoSizeExU(
	DWORD dwFlags,
	LPCSTR lpstrFilename,
	LPDWORD lpdwHandle
);
#undef GetFileVersionInfoSizeEx
#define GetFileVersionInfoSizeEx GetFileVersionInfoSizeExU

void version_init(void);
