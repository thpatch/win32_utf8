/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * shell32.dll functions.
  */

#pragma once

UINT WINAPI DragQueryFileU(
	HANDLE hDrop,
	UINT iFile,
	LPSTR lpszFile,
	UINT cch
);
#undef DragQueryFile
#define DragQueryFile DragQueryFileU

HICON WINAPI ExtractIconU(
	HINSTANCE hInst,
	LPCSTR lpszExeFileName,
	UINT nIconIndex
);
#undef ExtractIcon
#define ExtractIcon ExtractIconU

UINT WINAPI ExtractIconExU(
	LPCSTR lpszFile,
	int nIconIndex,
	HICON *phiconLarge,
	HICON *phiconSmall,
	UINT nIcons
);
#undef ExtractIconEx
#define ExtractIconEx ExtractIconExU

PIDLIST_ABSOLUTE WINAPI SHBrowseForFolderU(
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

BOOL WINAPI SHGetPathFromIDListU(
	PCIDLIST_ABSOLUTE pidl,
	LPSTR pszPath
);
#undef SHGetPathFromIDList
#define SHGetPathFromIDList SHGetPathFromIDListU
