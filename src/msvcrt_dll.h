/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * C runtime functions.
  */

#pragma once

FILE * __cdecl fopen_u(
	const char * _Filename,
	const char * _Mode
);
#undef fopen
#define fopen fopen_u
