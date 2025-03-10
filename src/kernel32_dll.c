/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * kernel32.dll functions.
  */

const w32u8_pair_t kernel32_pairs[] = {
	{"CopyFileA", CopyFileU},
	{"CopyFileExA", CopyFileExU},
	{"CreateDirectoryA", CreateDirectoryU},
	{"CreateFileA", CreateFileU},
	{"CreateFileMappingA", CreateFileMappingU},
	{"CreateProcessA", CreateProcessU},
	{"DeleteFileA", DeleteFileU},
	{"FindFirstFileA", FindFirstFileU},
	{"FindNextFileA", FindNextFileU},
	{"FormatMessageA", FormatMessageU},
	{"GetCurrentDirectoryA", GetCurrentDirectoryU},
	{"GetEnvironmentVariableA", GetEnvironmentVariableU},
	{"GetFileAttributesA", GetFileAttributesU},
	{"GetFileAttributesExA", GetFileAttributesExU},
	{"GetFullPathNameA", GetFullPathNameU},
	{"GetCommandLineA", GetCommandLineU},
	{"GetModuleFileNameA", GetModuleFileNameU},
	{"GetModuleHandleExA", GetModuleHandleExU},
	{"GetPrivateProfileIntA", GetPrivateProfileIntU},
	{"GetPrivateProfileStringA", GetPrivateProfileStringU},
	{"GetStartupInfoA", GetStartupInfoU},
	{"GetTempFileNameA", GetTempFileNameU},
	{"GetTempPathA", GetTempPathU},
	{"IsDBCSLeadByte", IsDBCSLeadByteFB},
	{"LoadLibraryA", LoadLibraryU},
	{"MoveFileA", MoveFileU},
	{"MoveFileExA", MoveFileExU},
	{"MoveFileWithProgressA", MoveFileWithProgressU},
	{"MultiByteToWideChar", MultiByteToWideCharU},
	{"OpenFileMappingA", OpenFileMappingU},
	{"ReadFile", ReadFileU},
	{"RemoveDirectoryA", RemoveDirectoryU},
	{"SetCurrentDirectoryA", SetCurrentDirectoryU},
	{"SetEnvironmentVariableA", SetEnvironmentVariableU},
	{"WideCharToMultiByte", WideCharToMultiByteU},
	{"WriteFile", WriteFileU},
	{"WritePrivateProfileStringA", WritePrivateProfileStringU},
	{ NULL }
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

BOOL WINAPI CopyFileU(
	LPCSTR lpExistingFileName,
	LPCSTR lpNewFileName,
	BOOL bFailIfExists
)
{
	return CopyFileExU(
		lpExistingFileName, lpNewFileName, NULL, NULL, NULL,
		bFailIfExists ? COPY_FILE_FAIL_IF_EXISTS : 0
	);
}

BOOL WINAPI CopyFileExU(
	LPCSTR lpExistingFileName,
	LPCSTR lpNewFileName,
	LPPROGRESS_ROUTINE lpProgressRoutine,
	LPVOID lpData,
	LPBOOL pbCancel,
	DWORD dwCopyFlags
)
{
	BOOL ret;
	WCHAR_T_DEC(lpExistingFileName);
	WCHAR_T_DEC(lpNewFileName);
	WCHAR_T_CONV(lpExistingFileName);
	WCHAR_T_CONV(lpNewFileName);
	ret = CopyFileExW(
		lpExistingFileName_w, lpNewFileName_w,
		lpProgressRoutine, lpData, pbCancel, dwCopyFlags
	);
	WCHAR_T_FREE(lpExistingFileName);
	WCHAR_T_FREE(lpNewFileName);
	return ret;
}

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
	WCHAR_T_CONV(lpPathName);

	// no, this isn't optimized away
	lpPathName_w_len = wcslen(lpPathName_w);
	// If the last character is a \\ or a /, the directory will be created
	// by the final CreateDirectory, and we don't want to create it here.
	// So we don't check for the last character.
	for(i = 0; i < lpPathName_w_len - 1; i++) {
		if(lpPathName_w[i] == L'\\' || lpPathName_w[i] == L'/') {
			wchar_t old_c = lpPathName_w[i + 1];
			lpPathName_w[i + 1] = L'\0';
			lpPathName_w[i] = L'/';
			ret = CreateDirectoryW(lpPathName_w, lpSecurityAttributes);
			lpPathName_w[i + 1] = old_c;
		}
	}
	// Final directory
	ret = CreateDirectoryW(lpPathName_w, lpSecurityAttributes);
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
	WCHAR_T_CONV(lpFileName);
	ret = CreateFileW(
		lpFileName_w, dwDesiredAccess, dwShareMode | FILE_SHARE_READ, lpSecurityAttributes,
		dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile
	);
	WCHAR_T_FREE(lpFileName);
	return ret;
}

HANDLE WINAPI CreateFileMappingU(
	HANDLE hFile,
	LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
	DWORD flProtect,
	DWORD dwMaximumSizeHigh,
	DWORD dwMaximumSizeLow,
	LPCSTR lpName
)
{
	size_t name_length = lpName ? strlen(lpName) + 1 : 0;
	VLA(wchar_t, name_w, name_length);
	if (lpName) {
		StringToUTF16(name_w, lpName, name_length);
		lpName = (LPCSTR)name_w;
	}
	HANDLE ret = CreateFileMappingW(
		hFile, lpFileMappingAttributes, flProtect,
		dwMaximumSizeHigh, dwMaximumSizeLow, (LPCWSTR)lpName
	);
	VLA_FREE(name_w);
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
	STARTUPINFOW lpSI_w;
	// At least the structure sizes are identical here
	memcpy(&lpSI_w, lpSI, sizeof(STARTUPINFOW));
	lpSI_w.lpReserved = NULL;

	size_t app_name_len = lpAppName ? strlen(lpAppName) + 1 : 0;
	size_t cmd_line_len = lpCmdLine ? strlen(lpCmdLine) + 1 : 0;
	size_t cur_dir_len = lpCurrentDirectory ? strlen(lpCurrentDirectory) + 1 : 0;
	size_t desktop_len = lpSI_w.lpDesktop ? strlen((char*)lpSI_w.lpDesktop) + 1 : 0;
	size_t title_len = lpSI_w.lpTitle ? strlen((char*)lpSI_w.lpTitle) + 1 : 0;

	size_t total_len = app_name_len + cmd_line_len + cur_dir_len + desktop_len + title_len;
	VLA(wchar_t, param_buffers, total_len);
	wchar_t* param_buffer_write = param_buffers;
	if (lpAppName) {
		size_t written = StringToUTF16(param_buffer_write, lpAppName, app_name_len);
		lpAppName = (LPCSTR)param_buffer_write;
		param_buffer_write += written;
	}
	if (lpCmdLine) {
		size_t written = StringToUTF16(param_buffer_write, lpCmdLine, cmd_line_len);
		lpCmdLine = (LPSTR)param_buffer_write;
		param_buffer_write += written;
	}
	if (lpCurrentDirectory) {
		size_t written = StringToUTF16(param_buffer_write, lpCurrentDirectory, cur_dir_len);
		lpCurrentDirectory = (LPCSTR)param_buffer_write;
		param_buffer_write += written;
	}
	if (lpSI_w.lpDesktop) {
		size_t written = StringToUTF16(param_buffer_write, (char*)lpSI_w.lpDesktop, desktop_len);
		lpSI_w.lpDesktop = param_buffer_write;
		param_buffer_write += written;
	}
	if (lpSI_w.lpTitle) {
		StringToUTF16(param_buffer_write, (char*)lpSI_w.lpTitle, title_len);
		lpSI_w.lpTitle = param_buffer_write;
	}

	BOOL ret = CreateProcessW(
		(LPCWSTR)lpAppName,
		(LPWSTR)lpCmdLine,
		lpProcessAttributes,
		lpThreadAttributes,
		bInheritHandles,
		dwCreationFlags,
		lpEnvironment,
		(LPCWSTR)lpCurrentDirectory,
		&lpSI_w,
		lpProcessInformation
	);

	VLA_FREE(param_buffers);
	return ret;
}

BOOL WINAPI DeleteFileU(
	LPCSTR lpFileName
)
{
	BOOL ret;
	WCHAR_T_DEC(lpFileName);
	WCHAR_T_CONV(lpFileName);
	ret = DeleteFileW(lpFileName_w);
	WCHAR_T_FREE(lpFileName);
	return ret;
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
	WCHAR_T_CONV(lpFileName);
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

typedef enum {
	VOA_VA,
	VOA_ARRAY
} va_or_array_t;

static void** voa_arg(va_list *va, unsigned int n, va_or_array_t type)
{
	if(type == VOA_VA) {
		va_list vacopy = *va;
		unsigned int i;
		for(i = 0; i < n; i++) {
			va_arg(vacopy, void*);
		}
		return (void**)vacopy;
	}
	return (void**)((char*)va + (n * sizeof(void*)));
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
	DWORD ret = 0;
	LPSTR* lppBuffer = (LPSTR*)lpBuffer;
	int allocating = dwFlags & FORMAT_MESSAGE_ALLOCATE_BUFFER;
	size_t lpBuffer_w_len;
	wchar_t *lpBuffer_w = NULL;
	wchar_t *source_w = NULL;
	int i = 0;
	int inserts_used = 0;
	wchar_t *inserts_w[99] = {0};
	va_or_array_t voa = (dwFlags & FORMAT_MESSAGE_ARGUMENT_ARRAY)
		? VOA_ARRAY
		: VOA_VA;

	if(lpSource && dwFlags & FORMAT_MESSAGE_FROM_STRING) {
		WCHAR_T_DECA(lpSource);
		WCHAR_T_CONV(lpSource);

		if(dwFlags & ~FORMAT_MESSAGE_IGNORE_INSERTS && Arguments) {
			const char *p = lpSource;
			/** Whenever a width and/or precision format specifications are
			  * used together with the va_list form, any following insert
			  * numbers must be decremented in the argument string. To cite
			  * the example on the MSDN page, a format specification that is
			  * meant to return "  Bi    Bob Bill" with the argument list
			  * [4, 2, "Bill", "Bob", 6, "Bill"] would have to be
			  *
			  *		"%1!*.*s! %3 %4!*s!"
			  *
			  * for a va_list and
			  *
			  *		"%1!*.*s! %4 %5!*s!"
			  *
			  * for a FORMAT_MESSAGE_ARGUMENT_ARRAY. Therefore, we need to add
			  * that drift value to the insert number ourselves in the former
			  * case.
			  */
			int insert_drift = 0;
			while(*p) {
				printf_format_t fmt;
				int insert;

				// Skip characters before '%'
				for(; *p && *p != '%'; p++);
				if(!*p) {
					break;
				}
				// *p == '%' here
				p++;

				// Single characters
				if(strchr("% .!nrt", *p)) {
					p++;
					continue;
				// Insert number
				} else if(*p >= '1' && *p <= '9') {
					insert = (*p++) - '0';
					if(*p >= '0' && *p <= '9') {
						insert = (insert * 10) + (*p++) - '0';
					}
					insert--;

					// printf format
					if(*p == '!') {
						p = printf_format_parse(&fmt, p + 1);
						if(*(p++) != '!') {
							// Something has to be wrong with the input string
							SetLastError(ERROR_INVALID_PARAMETER);
							goto end;
						}
					} else {
						fmt.argc_before_type = 0;
						fmt.type_size_in_ints = 1;
						fmt.type = 's';
					}
					insert += fmt.argc_before_type + insert_drift;
					if((fmt.type == 's' || fmt.type == 'S') && inserts_w[insert] == NULL) {
						void **argptr = voa_arg(Arguments, insert, voa);
						const char *src = *argptr;
						WCHAR_T_DECA(src);
						WCHAR_T_CONV(src);
						inserts_w[insert] = src_w;
						inserts_used = max(insert + 1, inserts_used);
						*argptr = inserts_w[insert];
					}
					if(fmt.argc_before_type && voa == VOA_VA) {
						insert_drift++;
					}
				}
			}
		}
		source_w = lpSource_w;
		lpSource = lpSource_w;
	}

	ret = FormatMessageW(
		(nSize ? FORMAT_MESSAGE_ALLOCATE_BUFFER : 0) | dwFlags,
		lpSource, dwMessageId, dwLanguageId,
		(!lpBuffer && allocating) ? NULL : (LPWSTR)&lpBuffer_w,
		nSize, Arguments
	);
	if(!ret) {
		if(allocating && lpBuffer) {
			*lppBuffer = NULL;
		}
		goto end;
	} else if(allocating) {
		nSize = max(ret * sizeof(char) * UTF8_MUL, nSize);
		*lppBuffer = LocalAlloc(0, nSize);
		lpBuffer = *lppBuffer;
	}
	// Apparently, we're only supposed to either put all or nothing into
	// the output buffer.
	lpBuffer_w_len = wcslen(lpBuffer_w) + 1;
	ret = WideCharToMultiByte(CP_UTF8, 0,
		lpBuffer_w, lpBuffer_w_len, NULL, 0, NULL, NULL
	);
	if(ret <= nSize) {
		WideCharToMultiByte(CP_UTF8, 0,
			lpBuffer_w, lpBuffer_w_len, lpBuffer, nSize, NULL, NULL
		);
		ret--; // excluding the terminating NULL character...
	} else {
		SetLastError(ERROR_INSUFFICIENT_BUFFER);
		ret = 0;
	}
end:
	for(i = 0; i < inserts_used; i++) {
		w32u8_freea(inserts_w[i]);
	}
	w32u8_freea(source_w);
	LocalFree(lpBuffer_w);
	return ret;
}

LPSTR WINAPI GetCommandLineU(
	VOID
)
{
	static char *command_line = NULL;
	if(!command_line) {
		const wchar_t *command_line_w = GetCommandLineW();
		WCSLEN_DEC(command_line_w);
		command_line = (char*)malloc(command_line_w_len);
		StringToUTF8(command_line, command_line_w, command_line_w_len);
	}
	return command_line;
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
	WCHAR_T_CONV(lpName);

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
	DWORD ret;
	WCHAR_T_DEC(lpFileName);
	WCHAR_T_CONV(lpFileName);
	ret = GetFileAttributesW(lpFileName_w);
	WCHAR_T_FREE(lpFileName);
	return ret;
}

BOOL WINAPI GetFileAttributesExU(
	LPCSTR lpFileName,
	GET_FILEEX_INFO_LEVELS fInfoLevelId,
	LPVOID lpFileInformation
)
{
	BOOL ret;
	WCHAR_T_DEC(lpFileName);
	WCHAR_T_CONV(lpFileName);
	ret = GetFileAttributesExW(lpFileName_w, fInfoLevelId, lpFileInformation);
	WCHAR_T_FREE(lpFileName);
	return ret;
}

DWORD WINAPI GetFullPathNameU(
	LPCSTR lpFileName,
	DWORD nBufferLength,
	LPSTR lpBuffer,
	LPSTR *lpFilePart
)
{
	LPWSTR lpwFilePart;
	DWORD ret;
	VLA(wchar_t, lpBuffer_w, nBufferLength);
	WCHAR_T_DEC(lpFileName);
	WCHAR_T_CONV(lpFileName);

	if (lpFilePart) {
		*lpFilePart = NULL;
	}
	if (!lpBuffer) {
		VLA_FREE(lpBuffer_w);
	}
	ret = GetFullPathNameW(lpFileName_w, nBufferLength, lpBuffer_w, &lpwFilePart);
	if (lpBuffer) {
		StringToUTF8(lpBuffer, lpBuffer_w, nBufferLength);
		if (lpFilePart && lpwFilePart) {
			*lpFilePart = lpBuffer + strlen(lpBuffer) - 1;
			while (*lpFilePart >= lpBuffer && **lpFilePart != '\\' && **lpFilePart != '/') {
				(*lpFilePart)--;
			}
			(*lpFilePart)++;
		}
	}
	else {
		// Hey, let's be nice and return the _actual_ length.
		VLA(wchar_t, lpBufferReal_w, ret);
		GetFullPathNameW(lpFileName_w, ret, lpBufferReal_w, NULL);
		ret = StringToUTF8(NULL, lpBufferReal_w, 0) + 1;
		VLA_FREE(lpBufferReal_w);
	}
	WCHAR_T_FREE(lpFileName);
	VLA_FREE(lpBuffer_w);
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

	// Only real VLAs can safely change size in a loop. Doing this with _malloca
	// or _alloca just continually expands the stack and never deallocates.

	DWORD wide_len = nSize ? nSize : MAX_PATH;
#if !VLA_SUPPORT
	wchar_t* lpFilename_w = (wchar_t*)malloc(wide_len * sizeof(wchar_t));
#endif
	for (;;) {
#if VLA_SUPPORT
		wchar_t lpFilename_w[wide_len];
#endif

		DWORD ret = GetModuleFileNameW(hModule, lpFilename_w, wide_len);
		if (ret) {
			if (ret == wide_len) {
				wide_len += MAX_PATH;
#if !VLA_SUPPORT
				lpFilename_w = (wchar_t*)realloc(lpFilename_w, wide_len * sizeof(wchar_t));
#endif
				continue;
			}
			// The last error and return value will be set correctly
			// by the WideCharToMultiByte call inside StringToUTF8
			ret = StringToUTF8(lpFilename, lpFilename_w, nSize);
#if !VLA_SUPPORT
			free(lpFilename_w);
#endif
		}
		return ret;
	}
}

BOOL WINAPI GetModuleHandleExU(
	DWORD dwFlags,
	LPCSTR lpFilename,
	HMODULE* hModule
)
{
	BOOL ret;
	// When GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS is set, [lpFilename] is an arbitrary address and not a string.
	if (!(dwFlags & GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS) && lpFilename != NULL) {
		WCHAR_T_DEC(lpFilename);
		WCHAR_T_CONV(lpFilename);
		ret = GetModuleHandleExW(dwFlags, lpFilename_w, hModule);
		WCHAR_T_FREE(lpFilename);
	} else {
		ret = GetModuleHandleExW(dwFlags, (void*)lpFilename, hModule);
	}
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

UINT WINAPI GetPrivateProfileStringU(
	LPCSTR lpAppName,
	LPCSTR lpKeyName,
	LPCSTR lpDefault,
	LPSTR lpReturnedString,
	DWORD nSize,
	LPCSTR lpFileName
)
{
	size_t app_name_len = lpAppName ? strlen(lpAppName) + 1 : 0;
	size_t key_name_len = lpKeyName ? strlen(lpKeyName) + 1 : 0;
	size_t default_len = lpDefault ? strlen(lpDefault) + 1 : 0;
	size_t file_name_len = strlen(lpFileName) + 1;

	size_t total_len = app_name_len + key_name_len + default_len + file_name_len + nSize;
	VLA(wchar_t, param_buffers, total_len);
	wchar_t* param_buffer_write = param_buffers;

	if (lpAppName) {
		size_t written = StringToUTF16(param_buffer_write, lpAppName, app_name_len);
		lpAppName = (LPCSTR)param_buffer_write;
		param_buffer_write += written;
	}
	if (lpKeyName) {
		size_t written = StringToUTF16(param_buffer_write, lpKeyName, key_name_len);
		lpKeyName = (LPCSTR)param_buffer_write;
		param_buffer_write += written;
	}

	// Yes, we can't just ignore it, pass NULL for [lpDefault] to the function
	// and memcpy() it ourselves if necessary. If [lpDefault] is NULL,
	// GetPrivateProfileString() just uses the empty string instead, and
	// there's no way of telling when it *did* use the default string.
	if (lpDefault) {
		size_t written = StringToUTF16(param_buffer_write, lpDefault, default_len);
		lpDefault = (LPCSTR)param_buffer_write;
		param_buffer_write += written;
	}

	size_t written = StringToUTF16(param_buffer_write, lpFileName, file_name_len);
	lpFileName = (LPCSTR)param_buffer_write;
	param_buffer_write += written;

	wchar_t* lpReturnedString_w = param_buffer_write;

	// Windows crashes in this case as well. Just like GetModuleFileName(),
	// this function can't retrieve the full length of the string anyway,
	// since it always null-terminates any truncated version of it. That only
	// leaves repeated checks with growing buffers... or a full-on custom
	// implementation of the functionality.
	assert(lpReturnedString);

	EnsurePrivateProfileUTF16((LPCWSTR)lpFileName);
	DWORD ret_w = GetPrivateProfileStringW(
		(LPCWSTR)lpAppName, (LPCWSTR)lpKeyName, (LPCWSTR)lpDefault,
		lpReturnedString_w, nSize, (LPCWSTR)lpFileName
	);

	UINT ret = 0;

	if(nSize) {
		ret = StringToMBFixed(
			lpReturnedString, lpReturnedString_w, nSize - 1, ret_w
		);
		lpReturnedString[ret] = 0;
		if((!lpAppName || !lpKeyName) && (ret + 1) == (nSize - 1)) {
			lpReturnedString[ret + 1] = 0;
		}
	}

	VLA_FREE(param_buffers);
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

UINT WINAPI GetTempFileNameU(
	LPCSTR lpPathName,
	LPCSTR lpPrefixString,
	UINT uUnique,
	LPSTR lpTempFileName
)
{
	UINT ret;
	WCHAR_T_DEC(lpPathName);
	WCHAR_T_DEC(lpPrefixString);
	VLA(wchar_t, lpFilename_w, MAX_PATH);

	WCHAR_T_CONV(lpPathName);
	WCHAR_T_CONV(lpPrefixString);
	ret = GetTempFileNameW(lpPathName_w, lpPrefixString_w, uUnique, lpFilename_w);
	if(ret) {
		StringToUTF8(lpTempFileName, lpFilename_w, MAX_PATH);
		if(GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
			ret = 0;
		}
	}
	VLA_FREE(lpFilename_w);
	WCHAR_T_FREE(lpPathName);
	WCHAR_T_FREE(lpPrefixString);
	return ret;
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
	return LoadLibraryExU(lpLibFileName, NULL, 0);
}

HMODULE WINAPI LoadLibraryExU(
	LPCSTR lpLibFileName,
	HANDLE hFile,
	DWORD dwFlags
)
{
	static int have_kb2533623 = -1;
	HMODULE ret;
	WCHAR_T_DEC(lpLibFileName);
	WCHAR_T_CONV(lpLibFileName);

	// Remove the flags that aren't supported without KB2533623.
	if(have_kb2533623 == -1) {
		have_kb2533623 = GetProcAddress(
			GetModuleHandleA("kernel32.dll"), "SetDefaultDllDirectories"
		) != 0;
	}
	if(!have_kb2533623) {
		dwFlags &= ~(
			LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR |
			LOAD_LIBRARY_SEARCH_APPLICATION_DIR |
			LOAD_LIBRARY_SEARCH_USER_DIRS |
			LOAD_LIBRARY_SEARCH_SYSTEM32 |
			LOAD_LIBRARY_SEARCH_DEFAULT_DIRS
		);
	}

	ret = LoadLibraryExW(lpLibFileName_w, hFile, dwFlags);
	WCHAR_T_FREE(lpLibFileName);
	return ret;
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
	WCHAR_T_CONV(lpExistingFileName);
	WCHAR_T_CONV(lpNewFileName);
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
	(void)CodePage;
	(void)dwFlags;

	int ret = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
		lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar
	);
	if(!ret && GetLastError() == ERROR_NO_UNICODE_TRANSLATION) {
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

HANDLE WINAPI OpenFileMappingU(
	DWORD dwDesiredAccess,
	BOOL bInheritHandle,
	LPCSTR lpName
)
{
	HANDLE ret;
	WCHAR_T_DEC(lpName);
	WCHAR_T_CONV(lpName);
	ret = OpenFileMappingW(dwDesiredAccess, bInheritHandle, lpName_w);
	WCHAR_T_FREE(lpName);
	return ret;
}

BOOL WINAPI RemoveDirectoryU(
	LPCSTR lpPathName
)
{
	BOOL ret;
	WCHAR_T_DEC(lpPathName);
	WCHAR_T_CONV(lpPathName);
	ret = RemoveDirectoryW(lpPathName_w);
	WCHAR_T_FREE(lpPathName);
	return ret;
}

BOOL WINAPI SetCurrentDirectoryU(
	LPCSTR lpPathName
)
{
	BOOL ret;
	WCHAR_T_DEC(lpPathName);
	WCHAR_T_CONV(lpPathName);
	ret = SetCurrentDirectoryW(lpPathName_w);
	WCHAR_T_FREE(lpPathName);
	return ret;
}

BOOL WINAPI SetEnvironmentVariableU(
	LPCSTR lpName,
	LPCSTR lpValue
)
{
	BOOL ret;
	WCHAR_T_DEC(lpName);
	WCHAR_T_DEC(lpValue);
	WCHAR_T_CONV(lpName);
	WCHAR_T_CONV(lpValue);
	ret = (BOOL)SetEnvironmentVariableW(lpName_w, lpValue_w);
	WCHAR_T_FREE(lpName);
	WCHAR_T_FREE(lpValue);
	return ret;
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
	(void)CodePage;
	(void)dwFlags;
	(void)lpDefaultChar;
	(void)lpUsedDefaultChar;

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

BOOL WINAPI ReadFileU(
	HANDLE hFile,
	LPVOID lpBuffer,
	DWORD nNumberOfBytesToRead,
	LPDWORD lpNumberOfBytesRead,
	LPOVERLAPPED lpOverlapped
)
{
	DWORD temp;
	if (!lpNumberOfBytesRead && !lpOverlapped) {
		lpNumberOfBytesRead = &temp;
	}
	return ReadFile(hFile, lpBuffer, nNumberOfBytesToRead,
		lpNumberOfBytesRead, lpOverlapped);
}

BOOL WINAPI WriteFileU(
	HANDLE hFile,
	LPCVOID lpBuffer,
	DWORD nNumberOfBytesToWrite,
	LPDWORD lpNumberOfBytesWritten,
	LPOVERLAPPED lpOverlapped
)
{
	DWORD temp;
	if (!lpNumberOfBytesWritten && !lpOverlapped) {
		lpNumberOfBytesWritten = &temp;
	}
	return WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite,
		lpNumberOfBytesWritten, lpOverlapped);
}

// Cleanup
void kernel32_exit(void)
{
	SAFE_FREE(startupinfo_desktop);
	SAFE_FREE(startupinfo_title);
}
