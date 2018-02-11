/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * shlwapi.dll functions.
  */

#pragma once

WRAPPER_DEC(BOOL STDAPICALLTYPE, PathFileExists,
	LPCSTR pszPath
);
#undef PathFileExists
#define PathFileExists PathFileExistsU

WRAPPER_DEC(LPSTR STDAPICALLTYPE, PathFindFileName,
	LPCSTR pszPath
);
#undef PathFindFileName
#define PathFindFileName PathFindFileNameU

WRAPPER_DEC(BOOL STDAPICALLTYPE, PathMatchSpec,
	LPCSTR pszFile,
	LPCSTR pszSpec
);
#undef PathMatchSpec
#define PathMatchSpec PathMatchSpecU

WRAPPER_DEC(BOOL STDAPICALLTYPE, PathRemoveFileSpec,
	LPSTR pszPath
);
#undef PathRemoveFileSpec
#define PathRemoveFileSpec PathRemoveFileSpecU
