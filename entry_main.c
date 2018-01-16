/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * main() entry point.
  * Compile or #include this file as part of your sources
  * if your program runs in the console subsystem.
  */

#include "src/entry.h"
#undef main

int __cdecl wmain(void)
{
	return win32_utf8_entry(win32_utf8_main);
}

// If both main() and wmain() are defined...
// • Visual Studio (or more specifically, LINK.EXE) defaults to main()
// • Pelles C defaults to wmain(), without even printing a "ambiguous entry
//   point" warning
// • MinGW/GCC doesn't care, and expects wmain() if you specify -municode,
//   and main() by default.
// Thus, we keep main() as a convenience fallback for GCC.

#ifndef _MSC_VER

int __cdecl main(void)
{
	return win32_utf8_entry(win32_utf8_main);
}

#endif
