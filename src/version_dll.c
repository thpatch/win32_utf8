/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * version.dll functions.
  */

/// Functions not available before Vista
/// ------------------------------------
typedef DWORD WINAPI DLL_FUNC_TYPE(version, GetFileVersionInfoSizeExW)(
	DWORD dwFlags,
	LPCWSTR lpstrFilename,
	LPDWORD lpdwHandle
);

typedef BOOL WINAPI DLL_FUNC_TYPE(version, GetFileVersionInfoExW)(
	DWORD dwFlags,
	LPCWSTR lpstrFilename,
	DWORD dwHandle,
	DWORD dwLen,
	LPVOID lpData
);

DLL_FUNC_DEF(version, GetFileVersionInfoSizeExW);
DLL_FUNC_DEF(version, GetFileVersionInfoExW);
/// ------------------------------------

const w32u8_pair_t version_pairs[] = {
	{"GetFileVersionInfoA", GetFileVersionInfoU},
	{"GetFileVersionInfoExA", GetFileVersionInfoExU},
	{"GetFileVersionInfoSizeA", GetFileVersionInfoSizeU},
	{"GetFileVersionInfoSizeExA", GetFileVersionInfoSizeExU},
	NULL
};

BOOL WINAPI GetFileVersionInfoU(
	LPCSTR lpstrFilename,
	DWORD dwHandle,
	DWORD dwLen,
	LPVOID lpData
)
{
	BOOL ret;
	WCHAR_T_DEC(lpstrFilename);
	WCHAR_T_CONV_VLA(lpstrFilename);
	ret = GetFileVersionInfoW(lpstrFilename_w, dwHandle, dwLen, lpData);
	WCHAR_T_FREE(lpstrFilename);
	return ret;
}

BOOL WINAPI GetFileVersionInfoExU(
	DWORD dwFlags,
	LPCSTR lpstrFilename,
	DWORD dwHandle,
	DWORD dwLen,
	LPVOID lpData
)
{
	BOOL ret;
	WCHAR_T_DEC(lpstrFilename);
	WCHAR_T_CONV_VLA(lpstrFilename);
	DLL_FUNC_CALL(version, GetFileVersionInfoExW,
		dwFlags, lpstrFilename_w, dwHandle, dwLen, lpData
	);
	WCHAR_T_FREE(lpstrFilename);
	return ret;
}

DWORD WINAPI GetFileVersionInfoSizeU(
	LPCSTR lpstrFilename,
	LPDWORD lpdwHandle
)
{
	BOOL ret;
	WCHAR_T_DEC(lpstrFilename);
	WCHAR_T_CONV_VLA(lpstrFilename);
	ret = GetFileVersionInfoSizeW(lpstrFilename_w, lpdwHandle);
	WCHAR_T_FREE(lpstrFilename);
	return ret;
}

DWORD WINAPI GetFileVersionInfoSizeExU(
	DWORD dwFlags,
	LPCSTR lpstrFilename,
	LPDWORD lpdwHandle
)
{
	DWORD ret;
	WCHAR_T_DEC(lpstrFilename);
	WCHAR_T_CONV_VLA(lpstrFilename);
	DLL_FUNC_CALL(version, GetFileVersionInfoSizeExW,
		dwFlags, lpstrFilename_w, lpdwHandle
	);
	WCHAR_T_FREE(lpstrFilename);
	return ret;
}

void version_init(void)
{
	HMODULE version = GetModuleHandleA("version.dll");
	if(version) {
		DLL_FUNC_GET(version, GetFileVersionInfoSizeExW);
		DLL_FUNC_GET(version, GetFileVersionInfoExW);
	}
}
