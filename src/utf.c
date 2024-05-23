/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * Unicode conversion functions.
  */

int StringToUTF16(wchar_t *str_w, const char *str_mb, int str_len)
{
	int str_len_w;
	if(!str_mb || !str_len) {
		return 0;
	}
	if(str_len == -1) {
		str_len = strlen(str_mb) + 1;
	}
	str_len_w = str_w ? str_len : 0;
	return MultiByteToWideCharU(0, 0, str_mb, str_len, str_w, str_len_w);
}

wchar_t* StringToUTF16_VLA(wchar_t *str_w, const char *str_mb, int str_len)
{
	if(str_mb) {
		StringToUTF16(str_w, str_mb, str_len);
		return str_w;
	}
	return NULL;
}

int StringToUTF8(char *str_utf8, const wchar_t *str_w, int str_utf8_len)
{
	int ret = WideCharToMultiByte(
		CP_UTF8, 0, str_w, -1, str_utf8, str_utf8_len, NULL, NULL
	);
	return str_w ? ret - 1 : ret;
}

int StringToMBFixed(char *str_mb, const wchar_t *str_w, int str_mb_len, int str_w_len)
{
	BOOL invalid = FALSE;
	extern UINT fallback_codepage;
	int ret = WideCharToMultiByte(
		fallback_codepage, WC_NO_BEST_FIT_CHARS,
		str_w, str_w_len, str_mb, str_mb_len, NULL, &invalid
	);
	if(!ret || invalid) {
		ret = WideCharToMultiByte(
			CP_UTF8, 0, str_w, str_w_len, str_mb, str_mb_len, NULL, NULL
		);
	}
	return ret;
}

UINT CharToUTF16(UINT c_mb)
{
	// Following the scheme used in existing DBCS code and Wine, I *guess*
	// this is how you would be supposed to pass UTF-8 multibyte characters
	// to GetGlyphOutlineA? Not that there's any code out there that actually
	// *does* this (without being forcefully hacked into it, that is, and
	// we'd need some test cases for that).
	// Anyway, this feels cleaner than using IsDBCSLeadByteEx().
	char c_a[sizeof(UINT) + 1] = { 0 };
	wchar_t c_w[sizeof(c_a)];
	UINT codepoint = 0;

	char *p = c_a;
	size_t i;
	for (i = 0; i < sizeof(UINT); i++) {
		size_t shift = (sizeof(UINT) - 1 - i) * 8;
		char byte = (c_mb & (0xFF << shift)) >> shift;
		if (byte) {
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
	if (c_w[0] < 0xD800 || c_w[0] >= 0xE000) {
		codepoint = c_w[0];
	}
	else {
		codepoint = ((c_w[0] - 0xD800) << 10) + (c_w[1] - 0xDC00) + 0x10000;
	}

	return codepoint;
}

char* EnsureUTF8(const char *str, int str_len)
{
	char *str_utf8 = NULL;
	if(str) {
		size_t str_w_len = str_len + 1;
		size_t str_utf8_len = str_w_len * UTF8_MUL;
		VLA(wchar_t, str_w, str_w_len);
		str_utf8 = (char*)malloc(str_utf8_len);
		ZeroMemory(str_w, str_w_len * sizeof(wchar_t));
		ZeroMemory(str_utf8, str_utf8_len * sizeof(char));
		WCHAR_T_CONV(str);
		StringToUTF8(str_utf8, str_w, str_utf8_len);
		VLA_FREE(str_w);
	}
	return str_utf8;
}
