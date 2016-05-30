/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * shlwapi.dll functions.
  */

#pragma once

BOOL STDAPICALLTYPE PathFileExistsU(
	LPCSTR pszPath
);
#undef PathFileExists
#define PathFileExists PathFileExistsU

BOOL STDAPICALLTYPE PathMatchSpecU(
	LPCSTR pszFile,
	LPCSTR pszSpec
);
#undef PathMatchSpec
#define PathMatchSpec PathMatchSpecU

BOOL STDAPICALLTYPE PathRemoveFileSpecU(
	LPSTR pszPath
);
#undef PathRemoveFileSpec
#define PathRemoveFileSpec PathRemoveFileSpecU
