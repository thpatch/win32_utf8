/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * kernel32.dll functions.
  */

#pragma once

BOOL WINAPI CopyFileU(
	LPCSTR lpExistingFileName,
	LPCSTR lpNewFileName,
	BOOL bFailIfExists
);
#undef CopyFile
#define CopyFile CopyFileU

BOOL WINAPI CopyFileExU(
	LPCSTR lpExistingFileName,
	LPCSTR lpNewFileName,
	LPPROGRESS_ROUTINE lpProgressRoutine,
	LPVOID lpData,
	LPBOOL pbCancel,
	DWORD dwCopyFlags
);
#undef CopyFileEx
#define CopyFileEx CopyFileExU

BOOL WINAPI CreateDirectoryU(
	LPCSTR lpPathName,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes
);
#undef CreateDirectory
#define CreateDirectory CreateDirectoryU

HANDLE WINAPI CreateFileU(
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

BOOL WINAPI CreateProcessU(
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

BOOL WINAPI DeleteFileU(
	LPCSTR lpFileName
);
#undef DeleteFile
#define DeleteFile DeleteFileU

HANDLE WINAPI FindFirstFileU(
	LPCSTR lpFileName,
	LPWIN32_FIND_DATAA lpFindFileData
);
#undef FindFirstFile
#define FindFirstFile FindFirstFileU

BOOL WINAPI FindNextFileU(
	HANDLE hFindFile,
	LPWIN32_FIND_DATAA lpFindFileData
);
#undef FindNextFile
#define FindNextFile FindNextFileU

DWORD WINAPI FormatMessageU(
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

LPSTR WINAPI GetCommandLineU(
	VOID
);
#undef GetCommandLine
#define GetCommandLine GetCommandLineU

DWORD WINAPI GetCurrentDirectoryU(
	DWORD nBufferLength,
	LPSTR lpBuffer
);
#undef GetCurrentDirectory
#define GetCurrentDirectory GetCurrentDirectoryU

DWORD WINAPI GetEnvironmentVariableU(
	LPCSTR lpName,
	LPSTR lpBuffer,
	DWORD nSize
);
#undef GetEnvironmentVariable
#define GetEnvironmentVariable GetEnvironmentVariableU

DWORD WINAPI GetFileAttributesU(
	LPCSTR lpFileName
);
#undef GetFileAttributes
#define GetFileAttributes GetFileAttributesU

BOOL WINAPI GetFileAttributesExU(
	LPCSTR lpFileName,
	GET_FILEEX_INFO_LEVELS fInfoLevelId,
	LPVOID lpFileInformation
);
#undef GetFileAttributesEx
#define GetFileAttributesEx GetFileAttributesExU

DWORD WINAPI GetModuleFileNameU(
	HMODULE hModule,
	LPSTR lpFilename,
	DWORD nSize
);
#undef GetModuleFileName
#define GetModuleFileName GetModuleFileNameU

UINT WINAPI GetPrivateProfileIntU(
	LPCSTR lpAppName,
	LPCSTR lpKeyName,
	INT nDefault,
	LPCSTR lpFileName
);
#undef GetPrivateProfileInt
#define GetPrivateProfileInt GetPrivateProfileIntU

VOID WINAPI GetStartupInfoU(
	LPSTARTUPINFOA lpStartupInfo
);
#undef GetStartupInfo
#define GetStartupInfo GetStartupInfoU

DWORD WINAPI GetTempPathU(
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

HMODULE WINAPI LoadLibraryU(
	LPCSTR lpLibFileName
);
#undef LoadLibrary
#define LoadLibrary LoadLibraryU

BOOL WINAPI MoveFileU(
	LPCSTR lpExistingFileName,
	LPCSTR lpNewFileName
);
#undef MoveFile
#define MoveFile MoveFileU

BOOL WINAPI MoveFileExU(
	LPCSTR lpExistingFileName,
	LPCSTR lpNewFileName,
	DWORD dwFlags
);
#undef MoveFileEx
#define MoveFileEx MoveFileExU

BOOL WINAPI MoveFileWithProgressU(
	LPCSTR lpExistingFileName,
	LPCSTR lpNewFileName,
	LPPROGRESS_ROUTINE lpProgressRoutine,
	LPVOID lpData,
	DWORD dwFlags
);
#undef MoveFileWithProgress
#define MoveFileWithProgress MoveFileWithProgressU

int WINAPI MultiByteToWideCharU(
	UINT CodePage,
	DWORD dwFlags,
	LPCSTR lpMultiByteStr,
	int cbMultiByte,
	LPWSTR lpWideCharStr,
	int cchWideChar
);

BOOL WINAPI RemoveDirectoryU(
	LPCSTR lpPathName
);
#undef RemoveDirectory
#define RemoveDirectory RemoveDirectoryU

BOOL WINAPI SetCurrentDirectoryU(
	LPCSTR lpPathName
);
#undef SetCurrentDirectory
#define SetCurrentDirectory SetCurrentDirectoryU

int WINAPI WideCharToMultiByteU(
	UINT CodePage,
	DWORD dwFlags,
	LPCWSTR lpWideCharStr,
	int cchWideChar,
	LPSTR lpMultiByteStr,
	int cbMultiByte,
	LPCSTR lpDefaultChar,
	LPBOOL lpUsedDefaultChar
);

BOOL WINAPI WritePrivateProfileStringU(
	LPCSTR lpAppName,
	LPCSTR lpKeyName,
	LPCSTR lpString,
	LPCSTR lpFileName
);
#undef WritePrivateProfileString
#define WritePrivateProfileString WritePrivateProfileStringU

// Cleanup
void kernel32_exit(void);
