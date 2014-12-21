/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * shell32.dll functions.
  */

#pragma once

UINT WINAPI DragQueryFileU(
	__in HANDLE hDrop,
	__in UINT iFile,
	__out_ecount_opt(cch) LPSTR lpszFile,
	__in UINT cch
);
#undef DragQueryFile
#define DragQueryFile DragQueryFileU

HICON WINAPI ExtractIconU(
	__reserved HINSTANCE hInst,
	__in LPCSTR lpszExeFileName,
	__in UINT nIconIndex
);
#undef ExtractIcon
#define ExtractIcon ExtractIconU

UINT WINAPI ExtractIconExU(
	LPCSTR lpszFile,
	int nIconIndex,
	__out_ecount_opt(nIcons) HICON *phiconLarge,
	__out_ecount_opt(nIcons) HICON *phiconSmall,
	UINT nIcons
);
#undef ExtractIconEx
#define ExtractIconEx ExtractIconExU

PIDLIST_ABSOLUTE WINAPI SHBrowseForFolderU(
	__in LPBROWSEINFOA lpbi
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
	__in PCIDLIST_ABSOLUTE pidl,
	__out_ecount(MAX_PATH) LPSTR pszPath
);
#undef SHGetPathFromIDList
#define SHGetPathFromIDList SHGetPathFromIDListU
