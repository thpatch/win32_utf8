/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * shlwapi.dll functions.
  */

const w32u8_pair_t shlwapi_pairs[] = {
	{"PathFileExistsA", PathFileExistsU},
	{"PathFindFileNameA", PathFindFileNameU},
	{"PathMatchSpecA", PathMatchSpecU},
	{"PathRemoveFileSpecA", PathRemoveFileSpecU},
	{ NULL }
};

BOOL STDAPICALLTYPE PathFileExistsU(
	LPCSTR pszPath
)
{
	return Wrap1P((Wrap1PFunc_t*)PathFileExistsW, pszPath);
}

LPSTR STDAPICALLTYPE PathFindFileNameU(
	LPCSTR pszPath
)
{
	LPCSTR ret = pszPath;
	while(pszPath && pszPath[0]) {
		char c0 = pszPath[0];
		char c1 = pszPath[1];
		if(
			(c0 == '\\' || c0 == '/' || c0 == ':')
			&& (c1 && c1 != '\\' && c1 != '/')
		) {
			ret = pszPath;
		}
		pszPath = CharNextU(pszPath);
	}
	return (LPSTR)ret;
}

BOOL STDAPICALLTYPE PathMatchSpecU(
	LPCSTR pszFile,
	LPCSTR pszSpec
)
{
	BOOL ret;
	WCHAR_T_DEC(pszFile);
	WCHAR_T_DEC(pszSpec);
	WCHAR_T_CONV(pszFile);
	WCHAR_T_CONV(pszSpec);
	ret = PathMatchSpecW(pszFile_w, pszSpec_w);
	WCHAR_T_FREE(pszFile);
	WCHAR_T_FREE(pszSpec);
	return ret;
}

BOOL STDAPICALLTYPE PathRemoveFileSpecU(
	LPSTR pszPath
)
{
	// Hey, let's re-write the function to also handle forward slashes
	// while we're at it!
	LPSTR newPath = PathFindFileNameU(pszPath);
	if((newPath) && (newPath != pszPath)) {
		newPath[0] = TEXT('\0');
		return 1;
	}
	return 0;
}
