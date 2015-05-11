/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * kernel32.dll functions.
  */

#include "win32_utf8.h"
#include "wrappers.h"

const w32u8_pair_t kernel32_pairs[] = {
	{"CreateDirectoryA", CreateDirectoryU},
	{"CreateFileA", CreateFileU},
	{"CreateProcessA", CreateProcessU},
	{"DeleteFileA", DeleteFileU},
	{"FindFirstFileA", FindFirstFileU},
	{"FindNextFileA", FindNextFileU},
	{"FormatMessageA", FormatMessageU},
	{"GetCurrentDirectoryA", GetCurrentDirectoryU},
	{"GetEnvironmentVariableA", GetEnvironmentVariableU},
	{"GetFileAttributesA", GetFileAttributesU},
	{"GetFileAttributesExA", GetFileAttributesExU},
	{"GetModuleFileNameA", GetModuleFileNameU},
	{"GetPrivateProfileIntA", GetPrivateProfileIntU},
	{"GetStartupInfoA", GetStartupInfoU},
	{"GetTempPathA", GetTempPathU},
	{"IsDBCSLeadByte", IsDBCSLeadByteFB},
	{"LoadLibraryA", LoadLibraryU},
	{"MoveFileA", MoveFileU},
	{"MoveFileExA", MoveFileExU},
	{"MoveFileWithProgressA", MoveFileWithProgressU},
	{"MultiByteToWideChar", MultiByteToWideCharU},
	{"RemoveDirectoryA", RemoveDirectoryU},
	{"SetCurrentDirectoryA", SetCurrentDirectoryU},
	{"WideCharToMultiByte", WideCharToMultiByteU},
	{"WritePrivateProfileStringA", WritePrivateProfileStringU},
	NULL
};

// GetStartupInfo
// --------------
static char *startupinfo_desktop = NULL;
static char *startupinfo_title = NULL;
// --------------

// INI conversion
// --------------
static BOOL EnsurePrivateProfileUTF16(LPCWSTR fn)
{
	// These are all supported encodings, at least according to Wine.
	const BYTE BOM_UTF16_LE[] = {0xFF, 0xFE};
	const BYTE BOM_UTF16_BE[] = {0xFE, 0xFF};
	const BYTE BOM_UTF8[] = {0xEF, 0xBB, 0xBF};

	BOOL ret = 0;
	DWORD byte_ret;
	LARGE_INTEGER file_size;
	HANDLE hHeap = GetProcessHeap();
	size_t cont_a_len;
	size_t cont_w_len;
	LPSTR cont_a = NULL;
	LPWSTR cont_w = NULL;
	HANDLE hFile = CreateFileW(
		fn, GENERIC_READ | GENERIC_WRITE, 0, NULL,
		OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL
	);
	if(hFile == INVALID_HANDLE_VALUE) {
		return 0;
	}
	if(
		GetFileSizeEx(hFile, &file_size)
		&& file_size.QuadPart > sizeof(BOM_UTF8)
	) {
		BYTE file_bom[3];
		cont_a_len = (size_t)file_size.QuadPart;
		if(!ReadFile(hFile, file_bom, sizeof(file_bom), &byte_ret, NULL)) {
			goto end;
		}
		// Nothing to do if we're UTF-16, but seek back if we're ANSI
		if(
			!memcmp(file_bom, BOM_UTF16_LE, sizeof(BOM_UTF16_LE))
			|| !memcmp(file_bom, BOM_UTF16_BE, sizeof(BOM_UTF16_BE))
		) {
			ret = 1;
			goto end;
		} else if(!memcmp(file_bom, BOM_UTF8, sizeof(BOM_UTF8))) {
			cont_a_len -= sizeof(BOM_UTF8);
		} else {
			SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
		}
		// OK, we have to do a full file conversion
		cont_a = HeapAlloc(hHeap, 0, cont_a_len * sizeof(char));
		cont_w = HeapAlloc(hHeap, 0, cont_a_len * sizeof(wchar_t));
		if(!cont_a || !cont_w) {
			goto end;
		}
		if(!ReadFile(hFile, cont_a, cont_a_len, &byte_ret, NULL)) {
			goto end;
		}
		cont_w_len = StringToUTF16(cont_w, cont_a, cont_a_len);
	}
	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	WriteFile(hFile, BOM_UTF16_LE, sizeof(BOM_UTF16_LE), &byte_ret, NULL);
	if(cont_w && cont_w_len) {
		WriteFile(hFile, cont_w, cont_w_len * sizeof(wchar_t), &byte_ret, NULL);
	}
	SetEndOfFile(hFile);
end:
	if(cont_a) {
		HeapFree(hHeap, 0, cont_a);
	}
	if(cont_w) {
		HeapFree(hHeap, 0, cont_w);
	}
	CloseHandle(hFile);
	return ret;
}
// --------------

BOOL WINAPI CreateDirectoryU(
	LPCSTR lpPathName,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes
)
{
	// Hey, let's make this recursive while we're at it.
	BOOL ret;
	size_t i;
	size_t lpPathName_w_len;
	WCHAR_T_DEC(lpPathName);
	WCHAR_T_CONV_VLA(lpPathName);

	// no, this isn't optimized away
	lpPathName_w_len = wcslen(lpPathName_w);
	for(i = 0; i < lpPathName_w_len; i++) {
		if(lpPathName_w[i] == L'\\' || lpPathName_w[i] == L'/') {
			wchar_t old_c = lpPathName_w[i + 1];
			lpPathName_w[i + 1] = L'\0';
			lpPathName_w[i] = L'/';
			ret = CreateDirectoryW(lpPathName_w, NULL);
			lpPathName_w[i + 1] = old_c;
		}
	}
	// Final directory
	ret = CreateDirectoryW(lpPathName_w, NULL);
	WCHAR_T_FREE(lpPathName);
	return ret;
}

HANDLE WINAPI CreateFileU(
	LPCSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile
)
{
	HANDLE ret;
	WCHAR_T_DEC(lpFileName);
	WCHAR_T_CONV_VLA(lpFileName);
	ret = CreateFileW(
		lpFileName_w, dwDesiredAccess, dwShareMode | FILE_SHARE_READ, lpSecurityAttributes,
		dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile
	);
	WCHAR_T_FREE(lpFileName);
	return ret;
}

BOOL WINAPI CreateProcessU(
	LPCSTR lpAppName,
	LPSTR lpCmdLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles,
	DWORD dwCreationFlags,
	LPVOID lpEnvironment,
	LPCSTR lpCurrentDirectory,
	LPSTARTUPINFOA lpSI,
	LPPROCESS_INFORMATION lpProcessInformation
)
{
	BOOL ret;
	STARTUPINFOW lpSI_w;
	WCHAR_T_DEC(lpAppName);
	WCHAR_T_DEC(lpCmdLine);
	WCHAR_T_DEC(lpCurrentDirectory);

	WCHAR_T_CONV_VLA(lpAppName);
	WCHAR_T_CONV_VLA(lpCmdLine);
	WCHAR_T_CONV_VLA(lpCurrentDirectory);

	if(lpSI) {
		// At least the structure sizes are identical here
		memcpy(&lpSI_w, lpSI, sizeof(STARTUPINFOW));
		if(lpSI->lpDesktop) {
			size_t si_lpDesktop_len = strlen(lpSI->lpDesktop) + 1;
			VLA(wchar_t, si_lpDesktopW, si_lpDesktop_len);
			StringToUTF16(si_lpDesktopW, lpSI->lpDesktop, si_lpDesktop_len);
			lpSI_w.lpDesktop = si_lpDesktopW;
		}
		if(lpSI->lpTitle) {
			size_t si_lpTitle_len = strlen(lpSI->lpTitle) + 1;
			VLA(wchar_t, si_lpTitleW, si_lpTitle_len);
			StringToUTF16(si_lpTitleW, lpSI->lpTitle, si_lpTitle_len);
			lpSI_w.lpDesktop = si_lpTitleW;
		}
	} else {
		ZeroMemory(&lpSI_w, sizeof(STARTUPINFOW));
	}
	// "Set this member to NULL before passing the structure to CreateProcess,"
	// MSDN says.
	lpSI_w.lpReserved = NULL;
	ret = CreateProcessW(
		lpAppName_w,
		lpCmdLine_w,
		lpProcessAttributes,
		lpThreadAttributes,
		bInheritHandles,
		dwCreationFlags,
		lpEnvironment,
		lpCurrentDirectory_w,
		&lpSI_w,
		lpProcessInformation
	);
	VLA_FREE(lpSI_w.lpDesktop);
	VLA_FREE(lpSI_w.lpTitle);
	WCHAR_T_FREE(lpAppName);
	WCHAR_T_FREE(lpCmdLine);
	WCHAR_T_FREE(lpCurrentDirectory);
	return ret;
}

BOOL WINAPI DeleteFileU(
	LPCSTR lpFileName
)
{
	return Wrap1P((Wrap1PFunc_t)DeleteFileW, lpFileName);
}

static void CopyFindDataWToA(
	LPWIN32_FIND_DATAA w32fd_a,
	LPWIN32_FIND_DATAW w32fd_w
	)
{
	w32fd_a->dwFileAttributes = w32fd_w->dwFileAttributes;
	w32fd_a->ftCreationTime = w32fd_w->ftCreationTime;
	w32fd_a->ftLastAccessTime = w32fd_w->ftLastAccessTime;
	w32fd_a->ftLastWriteTime = w32fd_w->ftLastWriteTime;
	w32fd_a->nFileSizeHigh = w32fd_w->nFileSizeHigh;
	w32fd_a->nFileSizeLow = w32fd_w->nFileSizeLow;
	w32fd_a->dwReserved0 = w32fd_w->dwReserved0;
	w32fd_a->dwReserved1 = w32fd_w->dwReserved1;
	StringToUTF8(w32fd_a->cFileName, w32fd_w->cFileName, sizeof(w32fd_a->cFileName));
	StringToUTF8(w32fd_a->cAlternateFileName, w32fd_w->cAlternateFileName, sizeof(w32fd_a->cAlternateFileName));
#ifdef _MAC
	w32fd_a->dwFileType = w32fd_w->dwReserved1;
	w32fd_a->dwCreatorType = w32fd_w->dwCreatorType;
	w32fd_a->wFinderFlags = w32fd_w->wFinderFlags;
#endif
}

HANDLE WINAPI FindFirstFileU(
	LPCSTR lpFileName,
	LPWIN32_FIND_DATAA lpFindFileData
)
{
	HANDLE ret;
	DWORD last_error;
	WIN32_FIND_DATAW lpFindFileDataW;

	WCHAR_T_DEC(lpFileName);
	WCHAR_T_CONV_VLA(lpFileName);
	ret = FindFirstFileW(lpFileName_w, &lpFindFileDataW);
	last_error = GetLastError();
	CopyFindDataWToA(lpFindFileData, &lpFindFileDataW);
	SetLastError(last_error);
	WCHAR_T_FREE(lpFileName);
	return ret;
}

BOOL WINAPI FindNextFileU(
	HANDLE hFindFile,
	LPWIN32_FIND_DATAA lpFindFileData
)
{
	BOOL ret;
	DWORD last_error;
	WIN32_FIND_DATAW lpFindFileDataW;

	ret = FindNextFileW(hFindFile, &lpFindFileDataW);
	last_error = GetLastError();
	CopyFindDataWToA(lpFindFileData, &lpFindFileDataW);
	SetLastError(last_error);
	return ret;
}

DWORD WINAPI FormatMessageU(
	DWORD dwFlags,
	LPCVOID lpSource,
	DWORD dwMessageId,
	DWORD dwLanguageId,
	LPSTR lpBuffer,
	DWORD nSize,
	va_list *Arguments
)
{
	wchar_t *lpBufferW = NULL;

	DWORD ret = FormatMessageW(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | dwFlags, lpSource,
		dwMessageId, dwLanguageId, (LPWSTR)&lpBufferW, nSize, Arguments
	);
	if(!ret) {
		return ret;
	}
	if(dwFlags & FORMAT_MESSAGE_ALLOCATE_BUFFER) {
		LPSTR* lppBuffer = (LPSTR*)lpBuffer;

		ret = max(ret * sizeof(char) * UTF8_MUL, nSize);

		*lppBuffer = LocalAlloc(0, ret);
		lpBuffer = *lppBuffer;
	} else {
		ret = min(ret, nSize);
	}
	ret = StringToUTF8(lpBuffer, lpBufferW, ret);
	LocalFree(lpBufferW);
	return ret;
}

DWORD WINAPI GetCurrentDirectoryU(
	DWORD nBufferLength,
	LPSTR lpBuffer
)
{
	return WrapGetString(GetCurrentDirectoryW, nBufferLength, lpBuffer);
}

DWORD WINAPI GetEnvironmentVariableU(
	LPCSTR lpName,
	LPSTR lpBuffer,
	DWORD nSize
)
{
	DWORD ret;
	WCHAR_T_DEC(lpName);
	VLA(wchar_t, lpBuffer_w, nSize);
	WCHAR_T_CONV_VLA(lpName);

	GetEnvironmentVariableW(lpName_w, lpBuffer_w, nSize);
	// Return the converted size (!)
	ret = StringToUTF8(lpBuffer, lpBuffer_w, nSize);
	VLA_FREE(lpBuffer_w);
	WCHAR_T_FREE(lpName);
	return ret;
}

DWORD WINAPI GetFileAttributesU(
	LPCSTR lpFileName
)
{
	return Wrap1P(GetFileAttributesW, lpFileName);
}

BOOL WINAPI GetFileAttributesExU(
	LPCSTR lpFileName,
	GET_FILEEX_INFO_LEVELS fInfoLevelId,
	LPVOID lpFileInformation
)
{
	BOOL ret;
	WCHAR_T_DEC(lpFileName);
	WCHAR_T_CONV_VLA(lpFileName);
	ret = GetFileAttributesExW(lpFileName_w, fInfoLevelId, lpFileInformation);
	WCHAR_T_FREE(lpFileName);
	return ret;
}

DWORD WINAPI GetModuleFileNameU(
	HMODULE hModule,
	LPSTR lpFilename,
	DWORD nSize
)
{
	/**
	  * And here we are, the most stupid Win32 API function I've seen so far.
	  *
	  * This wrapper adds the "GetCurrentDirectory functionality" the original
	  * function unfortunately lacks. Pass NULL for [lpFilename] or [nSize] to
	  * get the size required for a buffer to hold the module name in UTF-8.
	  *
	  * ... and unless there is any alternative function I don't know of, the
	  * only way to actually calculate this size is to repeatedly increase a
	  * buffer and to check whether that has been enough.
	  *
	  * In practice though, this length should never exceed MAX_PATH. I failed to
	  * create any test case where the path would be larger. But just in case it
	  * is or this becomes more frequent some day, the code is here.
	  */

	DWORD ret = nSize ? nSize : MAX_PATH;
	VLA(wchar_t, lpFilename_w, ret);

	if(lpFilename && nSize) {
		GetModuleFileNameW(hModule, lpFilename_w, nSize);
	} else {
		BOOL error = 1;
		while(error) {
			GetModuleFileNameW(hModule, lpFilename_w, ret);
			error = GetLastError() == ERROR_INSUFFICIENT_BUFFER;
			if(error) {
				VLA(wchar_t, lpFilename_VLA, ret += MAX_PATH);
				VLA_FREE(lpFilename_w);
				lpFilename_w = lpFilename_VLA;
			}
		}
		nSize = 0;
	}
	ret = StringToUTF8(lpFilename, lpFilename_w, nSize);
	VLA_FREE(lpFilename_w);
	return ret;
}

#define INI_MACRO_EXPAND(macro) \
	macro(lpAppName); \
	macro(lpKeyName); \
	macro(lpFileName)

UINT WINAPI GetPrivateProfileIntU(
	LPCSTR lpAppName,
	LPCSTR lpKeyName,
	INT nDefault,
	LPCSTR lpFileName
)
{
	UINT ret;
	INI_MACRO_EXPAND(WCHAR_T_DEC);
	INI_MACRO_EXPAND(WCHAR_T_CONV);
	ret = GetPrivateProfileIntW(lpAppName_w, lpKeyName_w, nDefault, lpFileName_w);
	INI_MACRO_EXPAND(WCHAR_T_FREE);
	return ret;
}

VOID WINAPI GetStartupInfoU(
	LPSTARTUPINFOA lpSI
)
{
	STARTUPINFOW si_w;
	GetStartupInfoW(&si_w);

	// I would have put this code into kernel32_init, but apparently
	// GetStartupInfoW is "not safe to be called inside DllMain".
	// So unsafe in fact that Wine segfaults when I tried it
	if(!startupinfo_desktop) {
		size_t lpDesktop_len = wcslen(si_w.lpDesktop) + 1;
		startupinfo_desktop = (char*)malloc(lpDesktop_len * UTF8_MUL * sizeof(char));
		StringToUTF8(startupinfo_desktop, si_w.lpDesktop, lpDesktop_len);
	}
	if(!startupinfo_title) {
		size_t lpTitle_len = wcslen(si_w.lpTitle) + 1;
		startupinfo_title = (char*)malloc(lpTitle_len * UTF8_MUL * sizeof(char));
		StringToUTF8(startupinfo_title, si_w.lpTitle, lpTitle_len);
	}
	memcpy(lpSI, &si_w, sizeof(STARTUPINFOA));
	lpSI->lpDesktop = startupinfo_desktop;
	lpSI->lpTitle = startupinfo_title;
}

DWORD WINAPI GetTempPathU(
	DWORD nBufferLength,
	LPSTR lpBuffer
)
{
	return WrapGetString(GetTempPathW, nBufferLength, lpBuffer);
}

BOOL WINAPI IsDBCSLeadByteFB(
	BYTE TestChar
)
{
	extern UINT fallback_codepage;
	return IsDBCSLeadByteEx(fallback_codepage, TestChar);
}

HMODULE WINAPI LoadLibraryU(
	LPCSTR lpLibFileName
)
{
	return (HMODULE)Wrap1P((Wrap1PFunc_t)LoadLibraryW, lpLibFileName);
}

BOOL WINAPI MoveFileU(
	LPCSTR lpExistingFileName,
	LPCSTR lpNewFileName
)
{
	return MoveFileEx(lpExistingFileName, lpNewFileName, MOVEFILE_COPY_ALLOWED);
}

BOOL WINAPI MoveFileExU(
	LPCSTR lpExistingFileName,
	LPCSTR lpNewFileName,
	DWORD dwFlags
)
{
	return MoveFileWithProgress(
		lpExistingFileName, lpNewFileName, NULL, NULL, dwFlags
	);
}

BOOL WINAPI MoveFileWithProgressU(
	LPCSTR lpExistingFileName,
	LPCSTR lpNewFileName,
	LPPROGRESS_ROUTINE lpProgressRoutine,
	LPVOID lpData,
	DWORD dwFlags
)
{
	BOOL ret;
	WCHAR_T_DEC(lpExistingFileName);
	WCHAR_T_DEC(lpNewFileName);
	WCHAR_T_CONV_VLA(lpExistingFileName);
	WCHAR_T_CONV_VLA(lpNewFileName);
	ret = MoveFileWithProgressW(
		lpExistingFileName_w, lpNewFileName_w, lpProgressRoutine, lpData, dwFlags
	);
	WCHAR_T_FREE(lpExistingFileName);
	WCHAR_T_FREE(lpNewFileName);
	return ret;
}

int WINAPI MultiByteToWideCharU(
	UINT CodePage,
	DWORD dwFlags,
	LPCSTR lpMultiByteStr,
	int cbMultiByte,
	LPWSTR lpWideCharStr,
	int cchWideChar
)
{
	int ret = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
		lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar
	);
	if(!ret) {
		extern UINT fallback_codepage;
		if(lpMultiByteStr[cbMultiByte - 1] != 0) {
			// The previous conversion attempt still lingers in [lpMultiByteStr].
			// If we don't clear it, garbage may show up at the end of the
			// converted string if the original string wasn't null-terminated...
			ZeroMemory(lpWideCharStr, cchWideChar * sizeof(wchar_t));
		}
		ret = MultiByteToWideChar(fallback_codepage, MB_PRECOMPOSED,
			lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar
		);
	}
	return ret;
}

BOOL WINAPI RemoveDirectoryU(
	LPCSTR lpPathName
)
{
	return Wrap1P((Wrap1PFunc_t)RemoveDirectoryW, lpPathName);
}

BOOL WINAPI SetCurrentDirectoryU(
	LPCSTR lpPathName
)
{
	return Wrap1P((Wrap1PFunc_t)SetCurrentDirectoryW, lpPathName);
}

int WINAPI WideCharToMultiByteU(
	UINT CodePage,
	DWORD dwFlags,
	LPCWSTR lpWideCharStr,
	int cchWideChar,
	LPSTR lpMultiByteStr,
	int cbMultiByte,
	LPCSTR lpDefaultChar,
	LPBOOL lpUsedDefaultChar
)
{
	return WideCharToMultiByte(
		CP_UTF8, 0, lpWideCharStr, cchWideChar,
		lpMultiByteStr, cbMultiByte, NULL, NULL
	);
}

BOOL WINAPI WritePrivateProfileStringU(
	LPCSTR lpAppName,
	LPCSTR lpKeyName,
	LPCSTR lpString,
	LPCSTR lpFileName
)
{
	BOOL ret;
	INI_MACRO_EXPAND(WCHAR_T_DEC);
	WCHAR_T_DEC(lpString);
	INI_MACRO_EXPAND(WCHAR_T_CONV);
	WCHAR_T_CONV(lpString);
	EnsurePrivateProfileUTF16(lpFileName_w);
	ret = WritePrivateProfileStringW(
		lpAppName_w, lpKeyName_w, lpString_w, lpFileName_w
	);
	INI_MACRO_EXPAND(WCHAR_T_FREE);
	WCHAR_T_CONV(lpString);
	return ret;
}

// Patcher functions
// -----------------
int kernel32_init(HMODULE hMod)
{
	return 0;
}

void kernel32_exit(void)
{
	SAFE_FREE(startupinfo_desktop);
	SAFE_FREE(startupinfo_title);
}
