/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * Unicode conversion functions.
  */

#pragma once

// Maximum length of the largest wchar_t in UTF-8
#define UTF8_MUL 4

/**
  * Converts [str_len] characters of the "narrow" string [str_mb] to UTF-16.
  * Input can either be in UTF-8 or the fallback codepage specified by a call
  * to w32u8_set_fallback_codepage().
  *
  * [str_len] can be -1 - this assumes that [str_mb] is null-terminated and
  * calculates the length automatically.
  *
  * If [str_w] and [str_len] are NULL/0, the function returns the required
  * length, in wide characters, for a buffer that fits [str_mb].
  */
int StringToUTF16(wchar_t *str_w, const char *str_mb, int str_len);

// StringToUTF16 for assigning VLA strings to optional structure members.
// Returns NULL if [str_mb] is NULL. Note that [str_w] still has to be freed
// by the caller in this case - which, however, it needs to do anyway if
// [str_mb] (and thus, [str_w]) point to a valid string.
wchar_t* StringToUTF16_VLA(wchar_t *str_w, const char *str_mb, int str_len);

// Converts a UTF-16 string to UTF-8.
// [str_utf8_len] takes the size of [str_utf8] in bytes.
int StringToUTF8(char *str_utf8, const wchar_t *str_w, int str_utf8_len);

// Converts the fixed-length string [str_w] to the fallback codepage or, if
// that conversion failed, to UTF-8. Useful in cases where applications
// concatenate strings returned from Windows functions with hardcoded strings
// in the application's native encoding.
int StringToMBFixed(char *str_mb, const wchar_t *str_w, int str_mb_len, int str_w_len);

// Returns [str] in UTF-8.
// Return value has to be free()d by the caller!
char* EnsureUTF8(const char *str, int str_len);
