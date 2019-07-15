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

WRAPPER_DEC(LPSTR STDAPICALLTYPE, PathAddBackslash,
	LPSTR pszPath
);
#undef PathAddBackslash
#define PathAddBackslash PathAddBackslashU

WRAPPER_DEC(BOOL STDAPICALLTYPE, PathIsDirectory,
	LPCSTR pszPath
);
#undef PathIsDirectory
#define PathIsDirectory PathIsDirectoryU

WRAPPER_DEC(BOOL STDAPICALLTYPE, PathIsRelative,
	LPCSTR pszPath
);
#undef PathIsRelative
#define PathIsRelative PathIsRelativeU

WRAPPER_DEC(BOOL STDAPICALLTYPE, PathCanonicalize,
	LPSTR pszBuf,
	LPCSTR pszPath
);
#undef PathCanonicalize
#define PathCanonicalize PathCanonicalizeU

WRAPPER_DEC(LPSTR STDAPICALLTYPE, PathCombine,
	LPSTR pszDest,
	LPCSTR pszDir,
	LPCSTR pszFile
);
#undef PathCombine
#define PathCombine PathCombineU

WRAPPER_DEC(BOOL STDAPICALLTYPE, PathAppend,
	LPSTR pszPath,
	LPCSTR pszMore
);
#undef PathAppend
#define PathAppend PathAppendU