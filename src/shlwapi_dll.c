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
	{"PathAddBackslashA", PathAddBackslashU},
	{"PathIsDirectoryA", PathIsDirectoryU},
	{"PathIsRelativeA", PathIsRelativeU},
	{"PathCanonicalizeA", PathCanonicalizeU},
	{"PathCombineA", PathCombineU},
	{"PathAppendA", PathAppendU},
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

LPSTR STDAPICALLTYPE PathAddBackslashU(
	LPSTR pszPath
)
{
	if (pszPath == 0 || *pszPath == '\x00') {
		return NULL;
	}
	size_t pszPathLen = strlen(pszPath);
	if (pszPath[pszPathLen - 1]  != '\\') {
		pszPath[pszPathLen] = '\\';
		pszPath[pszPathLen + 1] = '\x00';
		return (pszPath + pszPathLen + 1);
	} else {
		return (pszPath + pszPathLen);
	}
}

BOOL STDAPICALLTYPE PathIsDirectoryU(
	LPCSTR pszPath
)
{
	return Wrap1P((Wrap1PFunc_t*)PathIsDirectoryW, pszPath);
}

BOOL STDAPICALLTYPE PathIsRelativeU(
	LPCSTR pszPath
)
{
	return (*pszPath != '\\' && *pszPath != '/') && (*(pszPath + 1) != ':');
}

BOOL STDAPICALLTYPE PathCanonicalizeU(
	LPSTR pszBuf,
	LPCSTR pszPath
)
{	// This function may get reimplemented https://doxygen.reactos.org/de/dff/dll_2win32_2shlwapi_2path_8c.html#aa31be5d2410fbd8564ec0da929354a0f 
	wchar_t pszBuf_w[MAX_PATH];
	WCHAR_T_DEC(pszPath);
	WCHAR_T_CONV(pszPath);
	BOOL ret = PathCanonicalizeW(pszBuf_w, pszPath_w);

	WCHAR_T_FREE(pszPath);

	if (!ret) {
		return FALSE;
	}

	SetLastError(0);
	StringToUTF8(pszBuf, pszBuf_w, MAX_PATH);
	if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
		return FALSE;
	}

	return TRUE;
}

LPSTR STDAPICALLTYPE PathCombineU(
	LPSTR pszDest,
	LPCSTR pszDir,
	LPCSTR pszFile
)
{
	strcpy(pszDest, pszDir);
	if (PathIsRelativeU(pszFile)) {
		strcpy(PathAddBackslashU(pszDest), pszFile);
	}
	else {
		strcpy(pszDest, pszFile);
	}
	char final_buffer[MAX_PATH];
	BOOL ret = PathCanonicalizeU(final_buffer, pszDest);
	if (!ret) {
		return NULL;
	}
	strcpy(pszDest, final_buffer);
	return pszDest;
}

BOOL STDAPICALLTYPE PathAppendU(
	LPSTR pszPath,
	LPCSTR pszMore
)
{
	if (!PathCombineU(pszPath, pszPath, pszMore)) {
		return FALSE;
	}
	return TRUE;
}
