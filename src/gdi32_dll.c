/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * gdi32.dll functions.
  */

const w32u8_pair_t gdi32_pairs[] = {
	{"CreateFontA", CreateFontU},
	{"CreateFontIndirectA", CreateFontIndirectU},
	{"CreateFontIndirectExA", CreateFontIndirectExU},
    {"AddFontResourceExA", AddFontResourceExU},
    {"RemoveFontResourceExA", RemoveFontResourceExU},
    {"EnumFontsA", EnumFontFamiliesU},
	{"EnumFontFamiliesA", EnumFontFamiliesU},
	{"EnumFontFamiliesExA", EnumFontFamiliesExU},
	{"ExtTextOutA", ExtTextOutU},
	{"GetGlyphOutlineA", GetGlyphOutlineU},
	{"GetTextExtentPoint32A", GetTextExtentPoint32U},
	{"TextOutA", TextOutU},
	{ NULL }
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
)
{
	LOGFONTA lf_a = {
		cHeight, cWidth, cEscapement, cOrientation, cWeight, (BYTE)bItalic,
		(BYTE)bUnderline, (BYTE)bStrikeOut, (BYTE)iCharSet, (BYTE)iOutPrecision,
		(BYTE)iClipPrecision, (BYTE)iQuality, (BYTE)iPitchAndFamily, ""
	};
	// Yes, Windows does the same internally. CreateFont() is *not* a way
	// to pass a face name longer than 32 characters.
	if(pszFaceName) {
		strncpy(lf_a.lfFaceName, pszFaceName, sizeof(lf_a.lfFaceName));
	}
	return down_func(&lf_a);
}

HFONT WINAPI lower_CreateFontIndirectA(
	CreateFontIndirectExA_type *down_func,
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

int WINAPI lower_EnumFontFamiliesA(
	EnumFontFamiliesExA_type *down_func,
	HDC hdc,
	LPCSTR pszFaceName,
	FONTENUMPROCA lpProc,
	LPARAM lParam
)
{
	LOGFONTA lf;
	LOGFONTA *plf = NULL;

	if(pszFaceName) {
		if(!pszFaceName[0]) {
			return 1;
		}
		strncpy(lf.lfFaceName, pszFaceName, sizeof(lf.lfFaceName));
		lf.lfCharSet = DEFAULT_CHARSET;
		lf.lfPitchAndFamily = 0;
		plf = &lf;
	}
	return down_func(hdc, plf, lpProc, lParam, 0);
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

int WINAPI AddFontResourceExU(
    LPCSTR name,
    DWORD fl,
    PVOID res
)
{
    int ret;
    WCHAR_T_DEC(name);
    WCHAR_T_CONV(name);
    ret=AddFontResourceExW(name_w,fl,res);
    WCHAR_T_FREE(name);
    return ret;
}

int WINAPI RemoveFontResourceExU(
    LPCSTR name,
    DWORD fl,
    PVOID res
)
{
    int ret;
    WCHAR_T_DEC(name);
    WCHAR_T_CONV(name);
    ret=RemoveFontResourceExW(name_w,fl,res);
    WCHAR_T_FREE(name);
    return ret;
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

int WINAPI EnumFontFamiliesU(
	HDC hdc,
	LPCSTR pszFaceName,
	FONTENUMPROCA lpProc,
	LPARAM lParam
)
{
	return lower_EnumFontFamiliesA(EnumFontFamiliesExU,
		hdc, pszFaceName, lpProc, lParam
	);
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
	ret = ExtTextOutW(hdc, x, y, options, lprect, lpString_w, lpString_w_len, lpDx);
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
	return GetGlyphOutlineW(
		hdc, CharToUTF16(uChar), fuFormat, lpgm, cjBuffer, pvBuffer, lpmat2
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
	ret = GetTextExtentPoint32W(hdc, lpString_w, lpString_w_len, psizl);
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
