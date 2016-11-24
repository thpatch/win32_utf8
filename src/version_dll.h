/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * version.dll functions.
  */

#pragma once

WRAPPER_DEC(BOOL WINAPI, GetFileVersionInfo,
	LPCSTR lpstrFilename,
	DWORD dwHandle,
	DWORD dwLen,
	LPVOID lpData
);
#undef GetFileVersionInfo
#define GetFileVersionInfo GetFileVersionInfoU

WRAPPER_DEC(BOOL WINAPI, GetFileVersionInfoEx,
	DWORD dwFlags,
	LPCSTR lpstrFilename,
	DWORD dwHandle,
	DWORD dwLen,
	LPVOID lpData
);
#undef GetFileVersionInfoEx
#define GetFileVersionInfoEx GetFileVersionInfoExU

WRAPPER_DEC(DWORD WINAPI, GetFileVersionInfoSize,
	LPCSTR lpstrFilename,
	LPDWORD lpdwHandle
);
#undef GetFileVersionInfoSize
#define GetFileVersionInfoSize GetFileVersionInfoSizeU

WRAPPER_DEC(DWORD WINAPI, GetFileVersionInfoSizeEx,
	DWORD dwFlags,
	LPCSTR lpstrFilename,
	LPDWORD lpdwHandle
);
#undef GetFileVersionInfoSizeEx
#define GetFileVersionInfoSizeEx GetFileVersionInfoSizeExU

void version_init(void);
