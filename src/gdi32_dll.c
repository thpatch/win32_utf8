/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * gdi32.dll functions.
  */

#include "win32_utf8.h"

const w32u8_pair_t gdi32_pairs[] = {
	{"CreateFontA", CreateFontU},
	{"CreateFontIndirectA", CreateFontIndirectU},
	{"CreateFontIndirectExA", CreateFontIndirectExU},
	{"EnumFontFamiliesExA", EnumFontFamiliesExU},
	{"ExtTextOutA", ExtTextOutU},
	{"GetGlyphOutlineA", GetGlyphOutlineU},
	{"GetTextExtentPoint32A", GetTextExtentPoint32U},
	{"TextOutA", TextOutU},
	NULL
};

/// Font conversion helpers
/// -----------------------
static LOGFONTA* LogfontWToA(LOGFONTA *a, const LOGFONTW *w)
{
	if(w) {
		memcpy(a, w, offsetof(LOGFONT, lfFaceName));
		StringToUTF8(a->lfFaceName, w->lfFaceName, LF_FACESIZE);
	}
	return w ? a : NULL;
}

static LOGFONTW* LogfontAToW(LOGFONTW *w, const LOGFONTA *a)
{
	if(a) {
		// Limit the number of converted bytes to the actual length of
		// the face name, as any garbage past the 0 byte would cause
		// MultiByteToWideChar() with MB_ERR_INVALID_CHARS to fail.
		// This should maybe be done in StringToUTF16(), but I'm not
		// sure whether that would break something else...
		// (Fixes udm Self Extract Updater.)
		int facename_len = strnlen(a->lfFaceName, LF_FACESIZE) + 1;
		memcpy(w, a, offsetof(LOGFONT, lfFaceName));
		StringToUTF16(w->lfFaceName, a->lfFaceName, facename_len);
	}
	return a ? w : NULL;
}

static ENUMLOGFONTEXDVA* EnumLogfontExDVWToA(ENUMLOGFONTEXDVA *a, const ENUMLOGFONTEXDVW *w)
{
	if(w) {
		const ENUMLOGFONTEXW *elfe_w = &w->elfEnumLogfontEx;
		ENUMLOGFONTEXA *elfe_a = &a->elfEnumLogfontEx;
		// WinGDI.h says: "The actual size of the DESIGNVECTOR and
		// ENUMLOGFONTEXDV structures is determined by dvNumAxes,
		// MM_MAX_NUMAXES only detemines the maximal size allowed"
		DWORD dv_sizediff = (
			MM_MAX_NUMAXES - min(w->elfDesignVector.dvNumAxes, MM_MAX_NUMAXES)
		) * sizeof(LONG);

		LogfontWToA(&elfe_a->elfLogFont, &elfe_w->elfLogFont);
		StringToUTF8((char*)elfe_a->elfFullName, elfe_w->elfFullName, LF_FULLFACESIZE);
		StringToUTF8((char*)elfe_a->elfStyle, elfe_w->elfStyle, LF_FACESIZE);
		StringToUTF8((char*)elfe_a->elfScript, elfe_w->elfScript, LF_FACESIZE);
		memcpy(&a->elfDesignVector, &w->elfDesignVector, sizeof(DESIGNVECTOR) - dv_sizediff);
	}
	return w ? a : NULL;
}

static ENUMLOGFONTEXDVW* EnumLogfontExDVAToW(ENUMLOGFONTEXDVW *w, const ENUMLOGFONTEXDVA *a)
{
	if(w) {
		const ENUMLOGFONTEXA *elfe_a = &a->elfEnumLogfontEx;
		ENUMLOGFONTEXW *elfe_w = &w->elfEnumLogfontEx;
		DWORD dv_sizediff = (
			MM_MAX_NUMAXES - min(a->elfDesignVector.dvNumAxes, MM_MAX_NUMAXES)
		) * sizeof(LONG);

		LogfontAToW(&elfe_w->elfLogFont, &elfe_a->elfLogFont);
		StringToUTF16(elfe_w->elfFullName, (char*)elfe_a->elfFullName, LF_FULLFACESIZE);
		StringToUTF16(elfe_w->elfStyle, (char*)elfe_a->elfStyle, LF_FACESIZE);
		StringToUTF16(elfe_w->elfScript, (char*)elfe_a->elfScript, LF_FACESIZE);
		memcpy(&w->elfDesignVector, &a->elfDesignVector, sizeof(DESIGNVECTOR) - dv_sizediff);
	}
	return a ? w : NULL;
}

static ENUMTEXTMETRICA* EnumTextmetricWToA(ENUMTEXTMETRICA *a, const ENUMTEXTMETRICW *w)
{
	if(w) {
		const NEWTEXTMETRICW *ntm_w = &w->etmNewTextMetricEx.ntmTm;
		NEWTEXTMETRICA *ntm_a = &a->etmNewTextMetricEx.ntmTm;
		DWORD i = 0;

		memcpy(ntm_a, ntm_w, offsetof(NEWTEXTMETRIC, tmFirstChar));
		memcpy(&ntm_a->tmItalic, &ntm_w->tmItalic, sizeof(NEWTEXTMETRIC) - offsetof(NEWTEXTMETRIC, tmItalic));
		ntm_a->tmFirstChar = min(0xff, ntm_w->tmFirstChar);
		ntm_a->tmLastChar = min(0xff, ntm_w->tmLastChar);
		ntm_a->tmDefaultChar = min(0xff, ntm_w->tmDefaultChar);
		ntm_a->tmBreakChar = min(0xff, ntm_w->tmBreakChar);
		memcpy(&a->etmNewTextMetricEx.ntmFontSig, &w->etmNewTextMetricEx.ntmFontSig, sizeof(FONTSIGNATURE));
		memcpy(&a->etmAxesList, &w->etmAxesList, offsetof(AXESLIST, axlAxisInfo));
		for(i = 0; i < w->etmAxesList.axlNumAxes; i++) {
			const AXISINFOW *ai_w = &w->etmAxesList.axlAxisInfo[i];
			AXISINFOA *ai_a = &a->etmAxesList.axlAxisInfo[i];
			memcpy(ai_a, ai_w, offsetof(AXISINFO, axAxisName));
			StringToUTF8((char*)ai_a->axAxisName, ai_w->axAxisName, MM_MAX_AXES_NAMELEN);
		}
	}
	return w ? a : NULL;
}
/// -----------------------

/// Promotion wrappers
/// ------------------
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
)
{
	LOGFONTA lf_a = {
		cHeight, cWidth, cEscapement, cOrientation, cWeight, (BYTE)bItalic,
		(BYTE)bUnderline, (BYTE)bStrikeOut, (BYTE)iCharSet, (BYTE)iOutPrecision,
		(BYTE)iClipPrecision, (BYTE)iQuality, (BYTE)iPitchAndFamily
	};
	// Yes, Windows does the same internally. CreateFont() is *not* a way
	// to pass a face name longer than 32 characters.
	if(pszFaceName) {
		strncpy(lf_a.lfFaceName, pszFaceName, sizeof(lf_a.lfFaceName));
	}
	return down_func(&lf_a);
}

HFONT WINAPI lower_CreateFontIndirectA(
	CreateFontIndirectExA_type down_func,
	CONST LOGFONTA *lplf
)
{
	ENUMLOGFONTEXDVA elfedv_a;
	const size_t elfedv_lf_diff =
		sizeof(ENUMLOGFONTEXDVA) - offsetof(ENUMLOGFONTEXDVA, elfEnumLogfontEx.elfFullName)
	;
	if(!lplf) {
		return NULL;
	}
	memcpy(&elfedv_a.elfEnumLogfontEx.elfLogFont, lplf, sizeof(LOGFONTA));
	ZeroMemory(&elfedv_a.elfEnumLogfontEx.elfFullName, elfedv_lf_diff);
	return down_func(&elfedv_a);
}
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
)
{
	return lower_CreateFontA(CreateFontIndirectU,
		cHeight, cWidth, cEscapement, cOrientation, cWeight, bItalic,
		bUnderline, bStrikeOut, iCharSet, iOutPrecision,
		iClipPrecision, iQuality, iPitchAndFamily, pszFaceName
	);
}

HFONT WINAPI CreateFontIndirectU(
	CONST LOGFONTA *lplf
)
{
	return lower_CreateFontIndirectA(CreateFontIndirectExU, lplf);
}

HFONT WINAPI CreateFontIndirectExU(
	CONST ENUMLOGFONTEXDVA *lpelfe
)
{
	ENUMLOGFONTEXDVW elfedv_w;
	return CreateFontIndirectExW(EnumLogfontExDVAToW(&elfedv_w, lpelfe));
}

typedef struct {
	FONTENUMPROCA lpOrigProc;
	LPARAM lOrigParam;
} EnumFontFamExParam;

static int CALLBACK EnumFontFamExProcWrap(
	const ENUMLOGFONTEXDVW *lpelfe,
	const ENUMTEXTMETRICW *lpntme,
	DWORD FontType,
	EnumFontFamExParam *wrap_param
)
{
	if(wrap_param && wrap_param->lpOrigProc) {
		ENUMLOGFONTEXDVA elfedv_a;
		ENUMTEXTMETRICA etm_a;
		ENUMLOGFONTEXDVA *elfedv_a_ptr = EnumLogfontExDVWToA(&elfedv_a, lpelfe);
		ENUMTEXTMETRICA *etm_a_ptr = EnumTextmetricWToA(&etm_a, lpntme);
		return wrap_param->lpOrigProc(
			(LOGFONTA*)elfedv_a_ptr, (TEXTMETRICA*)etm_a_ptr, FontType, wrap_param->lOrigParam
		);
	}
	return 0;
}

int WINAPI EnumFontFamiliesExU(
	HDC hdc,
	LPLOGFONTA lpLogfont,
	FONTENUMPROCA lpProc,
	LPARAM lParam,
	DWORD dwFlags
)
{
	EnumFontFamExParam wrap_param = {lpProc, lParam};
	LOGFONTW lf_w;
	LOGFONTW *lf_w_ptr = LogfontAToW(&lf_w, lpLogfont);
	return EnumFontFamiliesExW(
		hdc, lf_w_ptr, (FONTENUMPROCW)EnumFontFamExProcWrap, (LPARAM)&wrap_param, dwFlags
	);
}

BOOL WINAPI ExtTextOutU(
	HDC hdc,
	int x,
	int y,
	UINT options,
	CONST RECT * lprect,
	LPCSTR lpString,
	UINT c,
	CONST INT * lpDx
)
{
	BOOL ret;
	FixedLengthStringConvert(lpString, c);
	ret = ExtTextOutW(hdc, x, y, options, lprect, lpString_w, wcslen(lpString_w), lpDx);
	WCHAR_T_FREE(lpString);
	return ret;
}

DWORD WINAPI GetGlyphOutlineU(
	HDC hdc,
	UINT uChar,
	UINT fuFormat,
	LPGLYPHMETRICS lpgm,
	DWORD cjBuffer,
	LPVOID pvBuffer,
	CONST MAT2 *lpmat2
)
{
	// Following the scheme used in existing DBCS code and Wine, I *guess*
	// this is how you would be supposed to pass UTF-8 multibyte characters
	// to this function? Not that there's any code out there that actually
	// *does* this (without being forcefully hacked into it, that is, and
	// we'd need some test cases for that).
	// Anyway, this feels cleaner than using IsDBCSLeadByteEx().
	char c_a[sizeof(UINT) + 1] = {0};
	wchar_t c_w[sizeof(c_a)];
	UINT codepoint = 0;

	char *p = c_a;
	for(int i = 0; i < sizeof(UINT); i++) {
		UINT shift = (sizeof(UINT) - 1 - i) * 8;
		char byte = (uChar & (0xFF << shift)) >> shift;
		if(byte) {
			*p++ = byte;
		}
	}
	StringToUTF16(c_w, c_a, sizeof(c_a));

	/**
	  * The nondescript "character" parameter of GetGlyphOutlineW() could
	  * either be interpreted as an UTF-32 code point or as UTF-16,
	  * using... well, *some* kind of byte order for surrogate pairs?
	  * As of now, it doesn't actually matter – the function does not
	  * support code points above U+FFFF, both interpretations are
	  * identical for code points below that, and nobody on the Internet
	  * seems to have any idea on how to use the W function for anything
	  * outside the Basic Multilingual Plane.
	  * Given the fact that this function has existed in this form (and
	  * probably hasn't been significantly altered) since Windows 3.1,
	  * where UINT was still a 16-bit type, this is hardly surprising.
	  *
	  * Nevertheless, I'd better do *something* here since the code would
	  * otherwise be obviously wrong, and the code point interpretation
	  * appears to be the more sensible one.
	  * (Which, given Microsoft's track record with Unicode, means
	  * that it's exactly the *wrong* thing to do...)
	  */
	if(c_w[0] < 0xD800 || c_w[0] >= 0xE000) {
		codepoint = c_w[0];
	} else {
		codepoint = ((c_w[0] - 0xD800) << 10) + (c_w[1] - 0xDC00) + 0x10000;
	}
	return GetGlyphOutlineW(
		hdc, codepoint, fuFormat, lpgm, cjBuffer, pvBuffer, lpmat2
	);
}

BOOL APIENTRY GetTextExtentPoint32U(
	HDC hdc,
	LPCSTR lpString,
	int c,
	LPSIZE psizl
)
{
	BOOL ret;
	FixedLengthStringConvert(lpString, c);
	ret = GetTextExtentPoint32W(hdc, lpString_w, wcslen(lpString_w), psizl);
	WCHAR_T_FREE(lpString);
	return ret;
}

BOOL WINAPI TextOutU(
	HDC hdc,
	int x,
	int y,
	LPCSTR lpString,
	int c
)
{
	return ExtTextOutU(hdc, x, y, 0, NULL, lpString, c, NULL);
}
