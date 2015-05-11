/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * C runtime functions.
  */

#include "win32_utf8.h"

const w32u8_pair_t msvcrt_pairs[] = {
	{"fopen", fopen_u},
	NULL
};

// Yes, this should better be implemented as a wrapper around fopen_s() (and
// thus, _wfopen_s()), but XP's msvcrt.dll doesn't have that function.
FILE * __cdecl fopen_u(
	const char * _Filename,
	const char * _Mode
)
{
	FILE *ret = NULL;
	WCHAR_T_DEC(_Filename);
	WCHAR_T_DEC(_Mode);
	WCHAR_T_CONV(_Filename);
	WCHAR_T_CONV(_Mode);
	ret = _wfopen(_Filename_w, _Mode_w);
	WCHAR_T_FREE(_Filename);
	WCHAR_T_FREE(_Mode);
	return ret;
}
