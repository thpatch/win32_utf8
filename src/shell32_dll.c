/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * shell32.dll functions.
  */

const w32u8_pair_t shell32_pairs[] = {
    {"ShellExecuteA", ShellExecuteU},
	{"DragQueryFileA", DragQueryFileU},
	{"ExtractIconA", ExtractIconU},
	{"ExtractIconExA", ExtractIconExU},
	{"SHBrowseForFolderA", SHBrowseForFolderU},
	{"SHGetFolderPathA", SHGetFolderPathU},
	{"SHGetPathFromIDListA", SHGetPathFromIDListU},
	{ NULL }
};

HINSTANCE WINAPI ShellExecuteU(HWND hwnd, 
    LPCSTR lpOperation, 
    LPCSTR lpFile, 
    LPCSTR lpParameters,
    LPCSTR lpDirectory,
    INT nShowCmd
)
{
    HINSTANCE ret;
    WCHAR_T_DEC(lpOperation);
    WCHAR_T_CONV(lpOperation);
    WCHAR_T_DEC(lpFile);
    WCHAR_T_CONV(lpFile);
    WCHAR_T_DEC(lpParameters);
    WCHAR_T_CONV(lpParameters);
    WCHAR_T_DEC(lpDirectory);
    WCHAR_T_CONV(lpDirectory);
    ret = ShellExecuteW(hwnd, lpOperation_w, lpFile_w,lpParameters_w,lpDirectory_w,nShowCmd);
    WCHAR_T_FREE(lpOperation);
    WCHAR_T_FREE(lpFile);
    WCHAR_T_FREE(lpParameters);
    WCHAR_T_FREE(lpDirectory);
    return ret;
}

LPSTR* WINAPI CommandLineToArgvU(
	LPCWSTR lpCmdLine,
	int* pNumArgs
)
{
	int cmd_line_pos; // Array "index" of the actual command line string
	WCSLEN_DEC(lpCmdLine);
	char **argv_u;

	wchar_t **argv_w = CommandLineToArgvW(lpCmdLine, pNumArgs);
	if(!argv_w) {
		return NULL;
	}
	cmd_line_pos = *pNumArgs + 1;

	// argv is indeed terminated with an additional sentinel NULL pointer.
	argv_u = LocalAlloc(
		LMEM_FIXED, cmd_line_pos * sizeof(char*) + lpCmdLine_len
	);
	if(argv_u) {
		int i;
		char *cur_arg_u = (char*)&argv_u[cmd_line_pos];
		for(i = 0; i < *pNumArgs; i++) {
			size_t cur_arg_u_len;
			argv_u[i] = cur_arg_u;
			cur_arg_u_len = StringToUTF8(
				cur_arg_u, argv_w[i], lpCmdLine_len
			) + 1;
			cur_arg_u += cur_arg_u_len;
			lpCmdLine_len -= cur_arg_u_len;
		}
		argv_u[i] = NULL;
	}

	LocalFree(argv_w);
	return argv_u;
}

UINT WINAPI DragQueryFileU(
	HANDLE hDrop,
	UINT iFile,
	LPSTR lpszFile,
	UINT cch
)
{
	DWORD ret;
	VLA(wchar_t, lpszFile_w, cch);

	if(!lpszFile) {
		VLA_FREE(lpszFile_w);
	}
	ret = DragQueryFileW(hDrop, iFile, lpszFile_w, cch);
	if(ret) {
		if(lpszFile) {
			StringToUTF8(lpszFile, lpszFile_w, cch);
		} else if(iFile != 0xFFFFFFFF) {
			VLA(wchar_t, lpBufferReal_w, ret);
			ret = DragQueryFileW(hDrop, iFile, lpBufferReal_w, cch);
			ret = StringToUTF8(NULL, lpBufferReal_w, 0);
			VLA_FREE(lpBufferReal_w);
		}
	}
	VLA_FREE(lpszFile_w);
	return ret;
}

HICON WINAPI ExtractIconU(
	HINSTANCE hInst,
	LPCSTR lpszExeFileName,
	UINT nIconIndex
)
{
	HICON ret;
	WCHAR_T_DEC(lpszExeFileName);
	WCHAR_T_CONV(lpszExeFileName);
	ret = ExtractIconW(hInst, lpszExeFileName_w, nIconIndex);
	WCHAR_T_FREE(lpszExeFileName);
	return ret;
}

UINT WINAPI ExtractIconExU(
	LPCSTR lpszFile,
	int nIconIndex,
	HICON *phiconLarge,
	HICON *phiconSmall,
	UINT nIcons
)
{
	UINT ret;
	WCHAR_T_DEC(lpszFile);
	WCHAR_T_CONV(lpszFile);
	ret = ExtractIconExW(
		lpszFile_w, nIconIndex, phiconLarge, phiconSmall, nIcons
	);
	WCHAR_T_FREE(lpszFile);
	return ret;
}

// CoGetApartmentType() is not available prior to Windows 7, but luckily,
// http://msdn.microsoft.com/en-us/library/windows/desktop/dd542641%28v=vs.85%29.aspx
// describes a way how to get the same functionality on previous systems.
static HRESULT CoGetApartmentTypeCompat(
	_Out_ APTTYPE *apttype
)
{
	int ret = S_FALSE;
	APTTYPEQUALIFIER apttype_qualifier;

	#ifdef __MINGW32__
		// Since adding -luuid causes MinGW to link in *all* GUIDs, we define
		// this manually, and therefore save ~17.5 KB in the compiled binary.
		const IID IID_IComThreadingInfo = {
			0x000001ce,0x0000,0x0000, { 0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x46 }
		};
	#endif

	typedef HRESULT WINAPI CoGetApartmentType_t(
		_Out_ APTTYPE *pAptType,
		_Out_ APTTYPEQUALIFIER *pAptQualifier
	);
	// GetVersionEx() is deprecated with Windows 8.1, not everybody will
	// have VersionHelpers.h, and this is a lot more fault-tolerant anyway.
	HMODULE ole32 = LoadLibrary("ole32.dll");
	CoGetApartmentType_t *cgat = NULL;

	*apttype = APTTYPE_MTA;
	if(!ole32) {
		return S_FALSE;
	}
	cgat = (CoGetApartmentType_t*)GetProcAddress(ole32, "CoGetApartmentType");
	if(cgat) {
		ret = cgat(apttype, &apttype_qualifier);
	} else {
		IUnknown *ctx_token = NULL;
		ret = CoGetContextToken((ULONG_PTR*)&ctx_token);
		if(ret == S_OK) {
			IComThreadingInfo *cti = NULL;
			ret = IUnknown_QueryInterface(
				ctx_token, &IID_IComThreadingInfo, (void**)&cti
			);
			if(ret == S_OK) {
				ret = IComThreadingInfo_GetCurrentApartmentType(cti, apttype);
				IUnknown_Release(cti);
			}
		} else if(ret == CO_E_NOTINITIALIZED) {
			*apttype = APTTYPE_CURRENT;
		}
	}
	FreeLibrary(ole32);
	return ret;
}

PIDLIST_ABSOLUTE WINAPI SHBrowseForFolderU(
	LPBROWSEINFOA lpbi
)
{
	APTTYPE apttype;
	PIDLIST_ABSOLUTE ret;
	wchar_t pszDisplayName_w[MAX_PATH];
	const char *lpszTitle = lpbi->lpszTitle;
	BROWSEINFOW lpbi_w = *((BROWSEINFOW*)lpbi);
	WCHAR_T_DEC(lpszTitle);
	WCHAR_T_CONV(lpszTitle);

	// Use the new UI if we can
	CoGetApartmentTypeCompat(&apttype);
	if(apttype != APTTYPE_MTA) {
		lpbi_w.ulFlags |= BIF_USENEWUI;
	}
	// Really, folder browse dialogs without edit box should be outlawed.
	lpbi_w.ulFlags |= BIF_EDITBOX;
	lpbi_w.pszDisplayName = pszDisplayName_w;
	lpbi_w.lpszTitle = lpszTitle_w;
	ret = SHBrowseForFolderW(&lpbi_w);
	StringToUTF8(lpbi->pszDisplayName, pszDisplayName_w, MAX_PATH);
	WCHAR_T_FREE(lpszTitle);
	return ret;
}

HRESULT WINAPI SHGetFolderPathU(
	HWND hWnd,
	int csidl,
	HANDLE hToken,
	DWORD dwFlags,
	LPSTR pszPath
)
{
	wchar_t pszPath_w[MAX_PATH];
	HRESULT ret;

	if(pszPath) {
		pszPath[0] = '\0';
	}

	ret = SHGetFolderPathW(hWnd, csidl, hToken, dwFlags, pszPath_w);
	if(ret == S_OK && pszPath) {
		StringToUTF8(pszPath, pszPath_w, MAX_PATH);
	}
	return ret;
}

BOOL WINAPI SHGetPathFromIDListU(
	PCIDLIST_ABSOLUTE pidl,
	LPSTR pszPath
)
{
	wchar_t pszPath_w[MAX_PATH];
	BOOL ret = SHGetPathFromIDListW(pidl, pszPath_w);
	if(pszPath) {
		StringToUTF8(pszPath, pszPath_w, MAX_PATH);
		return ret;
	}
	return 0;
}

HRESULT WINAPI SHParseDisplayNameU(
	LPCSTR pszName,
	IBindCtx *pbc,
	LPITEMIDLIST *ppidl,
	SFGAOF sfgaoIn,
	SFGAOF *psfgaoOut
)
{
	HRESULT ret;
	WCHAR_T_DEC(pszName);
	WCHAR_T_CONV(pszName);
	if(pszName_w) {
		wchar_t *p = pszName_w;
		while(*p) {
			if(*p == L'/') {
				*p = L'\\';
			}
			p++;
		}
	}
	ret = SHParseDisplayName(pszName_w, pbc, ppidl, sfgaoIn, psfgaoOut);
	WCHAR_T_FREE(pszName);
	return ret;
}
