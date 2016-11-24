/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * comdlg32.dll functions.
  */

#pragma once

WRAPPER_DEC(BOOL WINAPI, GetOpenFileName,
	LPOPENFILENAMEA ofn_a
);
#undef GetOpenFileName
#define GetOpenFileName GetOpenFileNameU

WRAPPER_DEC(BOOL WINAPI, GetSaveFileName,
	LPOPENFILENAMEA ofn_a
);
#undef GetSaveFileName
#define GetSaveFileName GetSaveFileNameU
