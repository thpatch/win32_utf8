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

int __cdecl main(void)
{
	return win32_utf8_entry(win32_utf8_main);
}
