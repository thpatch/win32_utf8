/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * psapi.dll functions.
  */

const w32u8_pair_t psapi_pairs[] = {
	{"GetModuleFileNameExA", GetModuleFileNameExU},
	NULL
};

DWORD WINAPI GetModuleFileNameExU(
	HANDLE hProcess,
	HMODULE hModule,
	LPSTR lpFilename,
	DWORD nSize
)
{
	VLA(wchar_t, lpFilename_w, nSize);
	DWORD ret = GetModuleFileNameExW(hProcess, hModule, lpFilename_w, nSize);
	StringToUTF8(lpFilename, lpFilename_w, nSize);
	VLA_FREE(lpFilename_w);
	return ret;
}
