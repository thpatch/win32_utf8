/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * gdi32.dll functions.
  */

#pragma once

WRAPPER_DEC(HFONT WINAPI, CreateFont,
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

WRAPPER_DEC(HFONT WINAPI, CreateFontIndirect,
	CONST LOGFONTA *lplf
);
#undef CreateFontIndirect
#define CreateFontIndirect CreateFontIndirectU

WRAPPER_DEC(HFONT WINAPI, CreateFontIndirectEx,
	CONST ENUMLOGFONTEXDVA *lpelfe
);
#undef CreateFontIndirectEx
#define CreateFontIndirectEx CreateFontIndirectExU

WRAPPER_DEC(int WINAPI, AddFontResourceEx,
    LPCSTR name,
    DWORD fl,
    PVOID res
);
#undef AddFontResourceEx
#define AddFontResourceEx AddFontResourceExU

WRAPPER_DEC(int WINAPI, RemoveFontResourceEx,
    LPCSTR name,
    DWORD fl,
    PVOID res
);
#undef RemoveFontResourceEx
#define RemoveFontResourceEx RemoveFontResourceExU

#undef EnumFonts
#define EnumFonts EnumFontFamiliesU

WRAPPER_DEC(int WINAPI, EnumFontFamilies,
	HDC hdc,
	LPCSTR pszFaceName,
	FONTENUMPROCA lpProc,
	LPARAM lParam
);
#undef EnumFontFamilies
#define EnumFontFamilies EnumFontFamiliesU

WRAPPER_DEC(int WINAPI, EnumFontFamiliesEx,
	HDC hdc,
	LPLOGFONTA lpLogfont,
	FONTENUMPROCA lpProc,
	LPARAM lParam,
	DWORD dwFlags
);
#undef EnumFontFamiliesEx
#define EnumFontFamiliesEx EnumFontFamiliesExU

WRAPPER_DEC(BOOL WINAPI, ExtTextOut,
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

WRAPPER_DEC(DWORD WINAPI, GetGlyphOutline,
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

WRAPPER_DEC(BOOL WINAPI, TextOut,
	HDC hdc,
	int x,
	int y,
	LPCSTR lpString,
	int c
);
#undef TextOut
#define TextOut TextOutU

/// Promotion wrappers
/// ------------------
HFONT WINAPI lower_CreateFontA(
	CreateFontIndirectA_type *down_func,
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
	CreateFontIndirectExA_type *down_func,
	CONST LOGFONTA *lplf
);

int WINAPI lower_EnumFontFamiliesA(
	EnumFontFamiliesExA_type *down_func,
	HDC hdc,
	LPCSTR lpLogfont,
	FONTENUMPROCA lpProc,
	LPARAM lParam
);
/// ------------------
