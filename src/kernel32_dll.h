/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * kernel32.dll functions.
  */

#pragma once

WRAPPER_DEC(BOOL WINAPI, CopyFile,
	LPCSTR lpExistingFileName,
	LPCSTR lpNewFileName,
	BOOL bFailIfExists
);
#undef CopyFile
#define CopyFile CopyFileU

WRAPPER_DEC(BOOL WINAPI, CopyFileEx,
	LPCSTR lpExistingFileName,
	LPCSTR lpNewFileName,
	LPPROGRESS_ROUTINE lpProgressRoutine,
	LPVOID lpData,
	LPBOOL pbCancel,
	DWORD dwCopyFlags
);
#undef CopyFileEx
#define CopyFileEx CopyFileExU

WRAPPER_DEC(BOOL WINAPI, CreateDirectory,
	LPCSTR lpPathName,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes
);
#undef CreateDirectory
#define CreateDirectory CreateDirectoryU

WRAPPER_DEC(HANDLE WINAPI, CreateFile,
	LPCSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile
);
#undef CreateFile
#define CreateFile CreateFileU

WRAPPER_DEC(HANDLE WINAPI, CreateFileMapping,
	HANDLE hFile,
	LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
	DWORD flProtect,
	DWORD dwMaximumSizeHigh,
	DWORD dwMaximumSizeLow,
	LPCSTR lpName
);
#undef CreateFileMapping
#define CreateFileMapping CreateFileMappingU

WRAPPER_DEC(BOOL WINAPI, CreateProcess,
	LPCSTR lpApplicationName,
	LPSTR lpCommandLine,
	PSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles,
	DWORD dwCreationFlags,
	LPVOID lpEnvironment,
	LPCSTR lpCurrentDirectory,
	LPSTARTUPINFOA lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation
);
#undef CreateProcess
#define CreateProcess CreateProcessU

WRAPPER_DEC(BOOL WINAPI, DeleteFile,
	LPCSTR lpFileName
);
#undef DeleteFile
#define DeleteFile DeleteFileU

WRAPPER_DEC(HANDLE WINAPI, FindFirstFile,
	LPCSTR lpFileName,
	LPWIN32_FIND_DATAA lpFindFileData
);
#undef FindFirstFile
#define FindFirstFile FindFirstFileU

WRAPPER_DEC(BOOL WINAPI, FindNextFile,
	HANDLE hFindFile,
	LPWIN32_FIND_DATAA lpFindFileData
);
#undef FindNextFile
#define FindNextFile FindNextFileU

WRAPPER_DEC(DWORD WINAPI, FormatMessage,
	DWORD dwFlags,
	LPCVOID lpSource,
	DWORD dwMessageId,
	DWORD dwLanguageId,
	LPSTR lpBuffer,
	DWORD nSize,
	va_list *Arguments
);
#undef FormatMessage
#define FormatMessage FormatMessageU

WRAPPER_DEC(LPSTR WINAPI, GetCommandLine,
	VOID
);
#undef GetCommandLine
#define GetCommandLine GetCommandLineU

WRAPPER_DEC(DWORD WINAPI, GetCurrentDirectory,
	DWORD nBufferLength,
	LPSTR lpBuffer
);
#undef GetCurrentDirectory
#define GetCurrentDirectory GetCurrentDirectoryU

WRAPPER_DEC(DWORD WINAPI, GetEnvironmentVariable,
	LPCSTR lpName,
	LPSTR lpBuffer,
	DWORD nSize
);
#undef GetEnvironmentVariable
#define GetEnvironmentVariable GetEnvironmentVariableU

WRAPPER_DEC(DWORD WINAPI, GetFileAttributes,
	LPCSTR lpFileName
);
#undef GetFileAttributes
#define GetFileAttributes GetFileAttributesU

WRAPPER_DEC(BOOL WINAPI, GetFileAttributesEx,
	LPCSTR lpFileName,
	GET_FILEEX_INFO_LEVELS fInfoLevelId,
	LPVOID lpFileInformation
);
#undef GetFileAttributesEx
#define GetFileAttributesEx GetFileAttributesExU

WRAPPER_DEC(DWORD WINAPI, GetFullPathName,
	LPCSTR lpFileName,
	DWORD nBufferLength,
	LPSTR lpBuffer,
	LPSTR *lpFilePart
);
#undef GetFullPathName
#define GetFullPathName GetFullPathNameU

WRAPPER_DEC(DWORD WINAPI, GetModuleFileName,
	HMODULE hModule,
	LPSTR lpFilename,
	DWORD nSize
);
#undef GetModuleFileName
#define GetModuleFileName GetModuleFileNameU

WRAPPER_DEC(UINT WINAPI, GetPrivateProfileInt,
	LPCSTR lpAppName,
	LPCSTR lpKeyName,
	INT nDefault,
	LPCSTR lpFileName
);
#undef GetPrivateProfileInt
#define GetPrivateProfileInt GetPrivateProfileIntU

WRAPPER_DEC(UINT WINAPI, GetPrivateProfileString,
	LPCSTR lpAppName,
	LPCSTR lpKeyName,
	LPCSTR lpDefault,
	LPSTR lpReturnedString,
	DWORD nSize,
	LPCSTR lpFileName
);
#undef GetPrivateProfileString
#define GetPrivateProfileString GetPrivateProfileStringU

WRAPPER_DEC(VOID WINAPI, GetStartupInfo,
	LPSTARTUPINFOA lpStartupInfo
);
#undef GetStartupInfo
#define GetStartupInfo GetStartupInfoU

WRAPPER_DEC(DWORD WINAPI, GetTempPath,
	DWORD nBufferLength,
	LPSTR lpBuffer
);
#undef GetTempPath
#define GetTempPath GetTempPathU

// Only implemented using the fallback codepage, since UTF-8 has
// no way to differentiate between continuation bytes and end bytes.
BOOL WINAPI IsDBCSLeadByteFB(
	BYTE TestChar
);
#undef IsDBCSLeadByte
#define IsDBCSLeadByte IsDBCSLeadByteFB

WRAPPER_DEC(HMODULE WINAPI, LoadLibrary,
	LPCSTR lpLibFileName
);
#undef LoadLibrary
#define LoadLibrary LoadLibraryU

// These will not be defined on any Visual Studio toolset that targets Windows
// XP; after all, if KB2533623 is not installed, LoadLibraryEx() with any of
// the search path flags would return NULL with GetLastError() == 87
// (ERROR_INVALID_PARAMETER). Therefore, LoadLibraryExU() clears them out
// automatically if KB2533623 isn't installed.
#undef  LOAD_WITH_ALTERED_SEARCH_PATH
#define LOAD_WITH_ALTERED_SEARCH_PATH       0x00000008
#undef  LOAD_IGNORE_CODE_AUTHZ_LEVEL
#define LOAD_IGNORE_CODE_AUTHZ_LEVEL        0x00000010
#undef  LOAD_LIBRARY_AS_IMAGE_RESOURCE
#define LOAD_LIBRARY_AS_IMAGE_RESOURCE      0x00000020
#undef  LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE
#define LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE  0x00000040
#undef  LOAD_LIBRARY_REQUIRE_SIGNED_TARGET
#define LOAD_LIBRARY_REQUIRE_SIGNED_TARGET  0x00000080
#undef  LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR
#define LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR    0x00000100
#undef  LOAD_LIBRARY_SEARCH_APPLICATION_DIR
#define LOAD_LIBRARY_SEARCH_APPLICATION_DIR 0x00000200
#undef  LOAD_LIBRARY_SEARCH_USER_DIRS
#define LOAD_LIBRARY_SEARCH_USER_DIRS       0x00000400
#undef  LOAD_LIBRARY_SEARCH_SYSTEM32
#define LOAD_LIBRARY_SEARCH_SYSTEM32        0x00000800
#undef  LOAD_LIBRARY_SEARCH_DEFAULT_DIRS
#define LOAD_LIBRARY_SEARCH_DEFAULT_DIRS    0x00001000

WRAPPER_DEC(HMODULE WINAPI, LoadLibraryEx,
	LPCSTR lpLibFileName,
	HANDLE hFile,
	DWORD dwFlags
);
#undef LoadLibraryEx
#define LoadLibraryEx LoadLibraryExU

WRAPPER_DEC(BOOL WINAPI, MoveFile,
	LPCSTR lpExistingFileName,
	LPCSTR lpNewFileName
);
#undef MoveFile
#define MoveFile MoveFileU

WRAPPER_DEC(BOOL WINAPI, MoveFileEx,
	LPCSTR lpExistingFileName,
	LPCSTR lpNewFileName,
	DWORD dwFlags
);
#undef MoveFileEx
#define MoveFileEx MoveFileExU

WRAPPER_DEC(BOOL WINAPI, MoveFileWithProgress,
	LPCSTR lpExistingFileName,
	LPCSTR lpNewFileName,
	LPPROGRESS_ROUTINE lpProgressRoutine,
	LPVOID lpData,
	DWORD dwFlags
);
#undef MoveFileWithProgress
#define MoveFileWithProgress MoveFileWithProgressU

WRAPPER_DEC(int WINAPI, MultiByteToWideChar,
	UINT CodePage,
	DWORD dwFlags,
	LPCSTR lpMultiByteStr,
	int cbMultiByte,
	LPWSTR lpWideCharStr,
	int cchWideChar
);

WRAPPER_DEC(HANDLE WINAPI, OpenFileMapping,
	DWORD dwDesiredAccess,
	BOOL bInheritHandle,
	LPCSTR lpName
);
#undef OpenFileMapping
#define OpenFileMapping OpenFileMappingU

WRAPPER_DEC(BOOL WINAPI, ReadFile,
	HANDLE hFile,
	LPVOID lpBuffer,
	DWORD nNumberOfBytesToRead,
	LPDWORD lpNumberOfBytesRead,
	LPOVERLAPPED lpOverlapped
);

WRAPPER_DEC(BOOL WINAPI, RemoveDirectory,
	LPCSTR lpPathName
);
#undef RemoveDirectory
#define RemoveDirectory RemoveDirectoryU

WRAPPER_DEC(BOOL WINAPI, SetCurrentDirectory,
	LPCSTR lpPathName
);
#undef SetCurrentDirectory
#define SetCurrentDirectory SetCurrentDirectoryU

WRAPPER_DEC(BOOL WINAPI, SetEnvironmentVariable,
	LPCSTR lpName,
	LPCSTR lpValue
);
#undef SetEnvironmentVariable
#define SetEnvironmentVariable SetEnvironmentVariableU

WRAPPER_DEC(int WINAPI, WideCharToMultiByte,
	UINT CodePage,
	DWORD dwFlags,
	LPCWSTR lpWideCharStr,
	int cchWideChar,
	LPSTR lpMultiByteStr,
	int cbMultiByte,
	LPCSTR lpDefaultChar,
	LPBOOL lpUsedDefaultChar
);

WRAPPER_DEC(BOOL WINAPI, WriteFile,
	HANDLE hFile,
	LPCVOID lpBuffer,
	DWORD nNumberOfBytesToWrite,
	LPDWORD lpNumberOfBytesWritten,
	LPOVERLAPPED lpOverlapped
);

WRAPPER_DEC(BOOL WINAPI, WritePrivateProfileString,
	LPCSTR lpAppName,
	LPCSTR lpKeyName,
	LPCSTR lpString,
	LPCSTR lpFileName
);
#undef WritePrivateProfileString
#define WritePrivateProfileString WritePrivateProfileStringU

// Cleanup
void kernel32_exit(void);
