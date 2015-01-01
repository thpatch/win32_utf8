/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * comdlg32.dll functions.
  */

#pragma once

BOOL WINAPI GetOpenFileNameU(
	LPOPENFILENAMEA ofn_a
);
#undef GetOpenFileName
#define GetOpenFileName GetOpenFileNameU

BOOL WINAPI GetSaveFileNameU(
	LPOPENFILENAMEA ofn_a
);
#undef GetSaveFileName
#define GetSaveFileName GetSaveFileNameU
