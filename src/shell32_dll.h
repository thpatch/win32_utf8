/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * shell32.dll functions.
  */

#pragma once

WRAPPER_DEC(HINSTANCE WINAPI, ShellExecute,
    HWND hwnd, 
    LPCSTR lpOperation, 
    LPCSTR lpFile, 
    LPCSTR lpParameters,
    LPCSTR lpDirectory,
    INT nShowCmd
);
#undef ShellExecute
#define ShellExecute ShellExecuteU

WRAPPER_DEC(LPSTR* WINAPI, CommandLineToArgv,
	LPCWSTR lpCmdLine,
	int* pNumArgs
);

WRAPPER_DEC(UINT WINAPI, DragQueryFile,
	HANDLE hDrop,
	UINT iFile,
	LPSTR lpszFile,
	UINT cch
);
#undef DragQueryFile
#define DragQueryFile DragQueryFileU

WRAPPER_DEC(HICON WINAPI, ExtractIcon,
	HINSTANCE hInst,
	LPCSTR lpszExeFileName,
	UINT nIconIndex
);
#undef ExtractIcon
#define ExtractIcon ExtractIconU

WRAPPER_DEC(UINT WINAPI, ExtractIconEx,
	LPCSTR lpszFile,
	int nIconIndex,
	HICON *phiconLarge,
	HICON *phiconSmall,
	UINT nIcons
);
#undef ExtractIconEx
#define ExtractIconEx ExtractIconExU

WRAPPER_DEC(PIDLIST_ABSOLUTE WINAPI, SHBrowseForFolder,
	LPBROWSEINFOA lpbi
);
#undef SHBrowseForFolder
#define SHBrowseForFolder SHBrowseForFolderU
#undef BROWSEINFO
#undef PBROWSEINFO
#undef LPBROWSEINFO
#define BROWSEINFO BROWSEINFOA
#define PBROWSEINFO PBROWSEINFOA
#define LPBROWSEINFO LPBROWSEINFOA

WRAPPER_DEC(HRESULT WINAPI, SHGetFolderPath,
	HWND hWnd,
	int csidl,
	HANDLE hToken,
	DWORD dwFlags,
	LPSTR pszPath
);
#undef SHGetFolderPath
#define SHGetFolderPath SHGetFolderPathU

WRAPPER_DEC(BOOL WINAPI, SHGetPathFromIDList,
	PCIDLIST_ABSOLUTE pidl,
	LPSTR pszPath
);
#undef SHGetPathFromIDList
#define SHGetPathFromIDList SHGetPathFromIDListU

WRAPPER_DEC(HRESULT WINAPI, SHParseDisplayName,
	LPCSTR pszName,
	IBindCtx *pbc,
	LPITEMIDLIST *ppidl,
	SFGAOF sfgaoIn,
	SFGAOF *psfgaoOut
);
