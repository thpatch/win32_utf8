/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * WinMain() entry point.
  * Compile or #include this file as part of your sources if your program runs
  * in the graphical Windows subsystem and should not open a console window.
  */

#include "src/entry.h"

int __stdcall wWinMain(
	// Avoid redefinition errors if wWinMain() was declared before, but don't
	// depend on it to keep compilation times low.
#ifdef _WINBASE_
	HINSTANCE hInstance,
	HINSTANCE hNull,
	LPWSTR lpCmdLine,
#else
	void *hInstance,
	void *hNull,
	unsigned short *lpCmdLine,
#endif
	int nCmdShow
)
{
	(void)hInstance;
	(void)hNull;
	(void)lpCmdLine;
	(void)nCmdShow;
	return win32_utf8_entry(win32_utf8_main);
}

// If both WinMain() and wWinMain() are defined...
// • Visual Studio (or more specifically, LINK.EXE) defaults to WinMain()
// • Pelles C defaults to wWinMain(), without even printing a "ambiguous
//   entry point" warning
// • MinGW/GCC doesn't care, and expects wWinMain() if you specify -municode
//   and -mwindows, and WinMain() if you only specify -mwindows.
// Thus, we keep WinMain() as a convenience fallback for GCC.

#ifndef _MSC_VER

int __stdcall WinMain(
	// Avoid redefinition errors if WinMain() was declared before, but don't
	// depend on it to keep compilation times low.
#ifdef _WINBASE_
	HINSTANCE hInstance,
	HINSTANCE hNull,
	LPSTR lpCmdLine,
#else
	void *hInstance,
	void *hNull,
	const char *lpCmdLine,
#endif
	int nCmdShow
)
{
	(void)hInstance;
	(void)hNull;
	(void)lpCmdLine;
	(void)nCmdShow;
	return win32_utf8_entry(win32_utf8_main);
}

#endif
