/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * gdi32.dll functions.
  */

#pragma once

/// Promotion wrappers
/// ------------------
typedef HFONT (WINAPI *CreateFontIndirectA_type)(CONST LOGFONTA*);
typedef HFONT (WINAPI *CreateFontIndirectExA_type)(CONST ENUMLOGFONTEXDVA*);

HFONT WINAPI lower_CreateFontA(
	CreateFontIndirectA_type down_func,
	int cHeight,
	int cWidth,
	int cEscapement,
	int cOrientation,
	int cWeight,
	DWORD bItalic,
	DWORD bUnderline,
	DWORD bStrikeOut,
	DWORD iCharSet,
	DWORD iOutPrecision,
	DWORD iClipPrecision,
	DWORD iQuality,
	DWORD iPitchAndFamily,
	LPCSTR pszFaceName
);

HFONT WINAPI lower_CreateFontIndirectA(
	CreateFontIndirectExA_type down_func,
	CONST LOGFONTA *lplf
);
/// ------------------

HFONT WINAPI CreateFontU(
	int cHeight,
	int cWidth,
	int cEscapement,
	int cOrientation,
	int cWeight,
	DWORD bItalic,
	DWORD bUnderline,
	DWORD bStrikeOut,
	DWORD iCharSet,
	DWORD iOutPrecision,
	DWORD iClipPrecision,
	DWORD iQuality,
	DWORD iPitchAndFamily,
	LPCSTR pszFaceName
);
#undef CreateFont
#define CreateFont CreateFontU

HFONT WINAPI CreateFontIndirectU(
	CONST LOGFONTA *lplf
);
#undef CreateFontIndirect
#define CreateFontIndirect CreateFontIndirectU

HFONT WINAPI CreateFontIndirectExU(
	CONST ENUMLOGFONTEXDVA *lpelfe
);
#undef CreateFontIndirectEx
#define CreateFontIndirectEx CreateFontIndirectExU

int WINAPI EnumFontFamiliesExU(
	HDC hdc,
	LPLOGFONTA lpLogfont,
	FONTENUMPROCA lpProc,
	LPARAM lParam,
	DWORD dwFlags
);
#undef EnumFontFamiliesEx
#define EnumFontFamiliesEx EnumFontFamiliesExU

BOOL WINAPI ExtTextOutU(
	HDC hdc,
	int x,
	int y,
	UINT options,
	CONST RECT * lprect,
	LPCSTR lpString,
	UINT c,
	CONST INT * lpDx
);
#undef ExtTextOut
#define ExtTextOut ExtTextOutU

DWORD WINAPI GetGlyphOutlineU(
	HDC hdc,
	UINT uChar,
	UINT fuFormat,
	LPGLYPHMETRICS lpgm,
	DWORD cjBuffer,
	LPVOID pvBuffer,
	CONST MAT2 *lpmat2
);
#undef GetGlyphOutline
#define GetGlyphOutline GetGlyphOutlineU

BOOL APIENTRY GetTextExtentPoint32U(
	HDC hdc,
	LPCSTR lpString,
	int c,
	LPSIZE psizl
);
#undef GetTextExtentPoint32
#define GetTextExtentPoint32 GetTextExtentPoint32U

BOOL WINAPI TextOutU(
	HDC hdc,
	int x,
	int y,
	LPCSTR lpString,
	int c
);
#undef TextOut
#define TextOut TextOutU
