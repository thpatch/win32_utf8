/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * psapi.dll functions.
  */

#pragma once

DWORD WINAPI GetModuleFileNameExU(
	HANDLE hProcess,
	HMODULE hModule,
	LPSTR lpFilename,
	DWORD nSize
);
#undef GetModuleFileNameEx
#define GetModuleFileNameEx GetModuleFileNameExU
