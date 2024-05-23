/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * Random macros and utility functions.
  */

#pragma once

// C versions
#ifdef __STDC__
# ifndef C89
#  define C89 1
# endif
# ifndef C90
#  define C90 1
# endif
# if defined(__STDC_VERSION__)
#  if (__STDC_VERSION__ >= 199409L)
#   ifndef C94
#    define C94 1
#   endif
// The proper name for this version is C95,
// but keep the C94 macro around just incase
#   ifndef C95
#    define C95 1
#   endif
#  endif
#  if !defined(C99) && (__STDC_VERSION__ >= 199901L)
#   define C99 1
#  endif
#  if !defined(C11) && (__STDC_VERSION__ >= 201112L)
#   define C11 1
#  endif
#  if !defined(C17) && (__STDC_VERSION__ >= 201710L)
#   define C17 1
#  endif
#  if !defined(C2X) && (__STDC_VERSION__ > 201710L)
#   define C2X 1
#  endif
# endif
#endif

#if defined(_WIN32) && defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#elif !C99
#include <malloc.h>
#endif

#include <stdlib.h>

#ifndef MACRO_WRAP
#define MACRO_WRAP(...) \
	do { \
		__VA_ARGS__ \
	} while (0)
#endif

/**
  * Resource identifier type. Either a multi-byte or wide string or, if the
  * high word is 0, an integer. Therefore, pretending that these are just
  * mere strings (as both their variable name *and* their Hungarian notation
  * prefix would suggest) is not only misleading, but downright harmful.
  *
  * I thought about using the completely type-safe approach of defining a
  * union for both string types. This would throw a compile error when a RESID
  * is accessed as if it were a string. However, there are pretty much only
  * drawbacks to that approach:
  * • This would exactly require to redeclare *all* structures to use the
  *   (W)RESID unions where applicable.
  * • Pretty much all of the A and W structures are typedef'd and thus can't
  *   easily be re-#define'd like the functions. This means that, in the case
  *   of static linking, these new declarations then wouldn't even propagate
  *   to existing application code.
  * • It would break compilation of existing, perfectly fine code as all
  *   function calls that pass a RESID union in one of their parameters or
  *   structures would also throw a compiler error.
  * • Finally, even if we only used the RESID union internally ourselves, it
  *   only adds complication to the conversion macros, which would need to
  *   validly convert the input "strings" into RESID unions.
  * So, it's just down to a mere typedef alias.
  */

typedef const char* RESID;
typedef const wchar_t* WRESID;

// Most Win32 API functions return TRUE on success and FALSE on failure,
// requiring a separate call to GetLastError() to get the actual error code.
// This macro wraps these function calls to use the opposite, more sensible
// scheme, returning FALSE on success and automatically calling GetLastError()
// on failure.
#define W32_ERR_WRAP(x) \
	((x) ? 0 : GetLastError())

// Freeing
#define SAFE_CLEANUP(func, x) MACRO_WRAP(\
	if (x) { \
		(void)func(x), (x) = 0; \
	} \
)
#define SAFE_FREE(x) SAFE_CLEANUP(free, x)

#define elementsof(x) (sizeof(x) / sizeof((x)[0]))

// Variable length arrays
#if C99 || C11 && (defined(__STDC_NO_VLA__) && __STDC_NO_VLA__ != 1)
# define VLA(type, name, size) \
	type name##_vla[(size)]; \
	type *name = name##_vla /* to ensure that [name] is a modifiable lvalue */
# define VLA_FREE(name) \
	do; while(0) /* require a semi-colon */
#else
# define VLA(type, name, size) \
	type *name = (type*)_malloca((size) * sizeof(type))
# define VLA_FREE(name) \
	SAFE_CLEANUP(_freea, name)
#endif

// Returns the length of a double-null-terminated string, not including the
// terminating two 0 bytes.
size_t zzstrlen(const char *str);

/// Convenient wchar_t conversion macros
/// ------------------------------------
#define STRLEN_DEC(src_char) \
	size_t src_char##_len = strlen(src_char) + 1

#define WCSLEN_DEC(src_wchar) \
	size_t src_wchar##_len = (wcslen(src_wchar) * UTF8_MUL) + 1

// "create-wchar_t-from-strlen"
#define WCHAR_T_DEC(src_char) \
	STRLEN_DEC(src_char); \
	VLA(wchar_t, src_char##_w, src_char##_len)

#define WCHAR_T_CONV(src_char) MACRO_WRAP(\
	if((src_char) == NULL) { \
		VLA_FREE(src_char##_w); \
	} else { \
		StringToUTF16(src_char##_w, src_char, src_char##_len); \
	} \
)

#define WCHAR_T_FREE(src_char) \
	VLA_FREE(src_char##_w)

// "create-UTF-8-from-wchar_t"
#define UTF8_DEC(src_wchar) \
	WCSLEN_DEC(src_wchar); \
	VLA(char, src_wchar##_utf8, src_wchar##_len)

#define UTF8_CONV(src_wchar) \
	StringToUTF8(src_wchar##_utf8, src_wchar, src_wchar##_len)

#define UTF8_FREE(src_wchar) \
	VLA_FREE(src_wchar##_utf8)
/// ------------------------------------

// Declare a wrapping function together with a corresponding typedef
#define WRAPPER_DEC(rettype, name, ...) \
	typedef rettype name##A_type(__VA_ARGS__); \
	rettype name##U(__VA_ARGS__)

/// Convenient dynamic binding for functions not available before Vista
/// -------------------------------------------------------------------
#define DLL_FUNC(dll, func) \
	dll##_##func

#define DLL_FUNC_TYPE(dll, func) \
	dll##_##func##_t /* GCC doesn't accept DLL_FUNC(dll_func)##_t */

#define DLL_FUNC_DEF(dll, func) \
	DLL_FUNC_TYPE(dll, func) *DLL_FUNC(dll, func) = NULL

#define DLL_FUNC_GET(dll, func) \
	DLL_FUNC(dll, func) = (DLL_FUNC_TYPE(dll, func)*)GetProcAddress(dll, #func)

#define DLL_FUNC_CALL(dll, func, ...) MACRO_WRAP(\
	if(DLL_FUNC(dll, func)) { \
		ret = DLL_FUNC(dll, func)(__VA_ARGS__); \
	} else { \
		MessageBoxU(NULL, \
			"Tried to call "#func"() from "#dll".dll, " \
			"which is not available on this Windows version.", \
			"Win32 UTF-8 wrapper", \
			MB_OK | MB_ICONEXCLAMATION \
		); \
		SetLastError(ERROR_CALL_NOT_IMPLEMENTED); \
		ret = 0; \
	} \
)
/// -------------------------------------------------------------------

/// IUnknown implementation for COM proxy classes
/// ---------------------------------------------
// Also comes with its own reference counting, in case someone needs to wrap
// a NULL instance.
#define IUNKNOWN_DEC(prefix, x) \
protected: \
	x *pOrig; \
	ULONG fallback_ref; \
\
public: \
	prefix##_##x(x *_pOrig) : pOrig(_pOrig), fallback_ref(1) {} \
\
	HRESULT __stdcall QueryInterface(REFIID riid, LPVOID * ppvObj); \
	ULONG __stdcall AddRef(); \
	ULONG __stdcall Release();

#define IUNKNOWN_DEF(x, QueryInterface_ReplaceCondition) \
	HRESULT x::QueryInterface(REFIID riid, void** ppvObj) \
	{ \
		if(!ppvObj) { \
			return E_POINTER; \
		} \
		if(!pOrig) { \
			return E_NOINTERFACE; \
		} \
		*ppvObj = NULL; \
		\
		HRESULT hRes = pOrig->QueryInterface(riid, ppvObj); \
		if(hRes == NOERROR && (QueryInterface_ReplaceCondition)) { \
			*ppvObj = this; \
		} \
		return hRes; \
	} \
	\
	ULONG x::AddRef() \
	{ \
		return !pOrig ? ++fallback_ref : pOrig->AddRef(); \
	} \
	\
	ULONG x::Release() \
	{ \
		ULONG count = !pOrig ? --fallback_ref : pOrig->Release(); \
		if(count == 0) { \
			delete this; \
		} \
		return count; \
	}
/// ---------------------------------------------

// Define Visual C++ warnings away
#if (_MSC_VER >= 1600) && \
    ((( defined _CRT_DECLARE_NONSTDC_NAMES && _CRT_DECLARE_NONSTDC_NAMES) || \
      (!defined _CRT_DECLARE_NONSTDC_NAMES && !__STDC__                 )) && \
    !(defined _CRT_NONSTDC_NO_DEPRECATE  && !defined _CRT_NONSTDC_NO_WARNINGS))
// char* itoa(int _Value, char *_Buffer, int _Radix)
# define itoa _itoa
#ifndef strdup
// char* strdup(const char *_Source)
# define strdup _strdup
#endif
// void* memccpy(void *_Dst, const void *_Src, int _Val, size_t _MaxCount)
# define memccpy _memccpy
// int strnicmp(const char *_String1, const char *_String2, size_t _MaxCount)
# define strnicmp _strnicmp
// int stricmp(const char *_String1, const char *_String2)
# define stricmp _stricmp
// char* strlwer(char *_String)
# define strlwr _strlwr
// int vsnwprintf(wchar_t *_Buffer, size_t _BufferCount, const wchar_t *_Format, va_list _ArgList)
# define vsnwprintf _vsnwprintf
// int wcsicmp(const wchar_t *_String1, const wchar_t *_String2)
# define wcsicmp _wcsicmp
#endif

// Convenience macro to convert one fixed-length string to UTF-16.
#define FixedLengthStringConvert(str_in, str_len) \
	size_t str_in##_len = (str_len != -1 ? str_len : strlen(str_in)); \
	int str_in##_w_len; \
	VLA(wchar_t, str_in##_w, str_in##_len + 1); \
	str_in##_w_len = StringToUTF16(str_in##_w, str_in, str_in##_len); \
	str_in##_w[str_in##_w_len] = L'\0'

// Now, if Microsoft just had used integer identifiers for resources instead
// of names plus the MAKEINTRESOURCE / MAKEINTATOM hacks, we could just
// point all these calls to their wide versions and be done with it.
// Instead, there is some maintenance to do...
#define RESID_DEC(local) \
	LPWSTR local##_w = NULL

#define RESID_CONV(local, src) MACRO_WRAP(\
	if(HIWORD(src) != 0) { \
		size_t local##_len = strlen(src) + 1; \
		VLA(wchar_t, local##_w_vla, local##_len); \
		local##_w = StringToUTF16_VLA(local##_w_vla, src, local##_len); \
	} else { \
		local##_w = (LPWSTR)(src); \
	} \
)

#define RESID_FREE(local, src) MACRO_WRAP(\
	if(HIWORD(src) != 0) { \
		WCHAR_T_FREE(local); \
	} \
)

/// printf format specifier parsing
/// -------------------------------
// Information about a single printf format specification.
typedef struct {
	int argc_before_type;
	char type;
	int type_size_in_ints;
} printf_format_t;

// Fills [fmt] with information about the printf format specification
// starting at [p]. [p] is assumed to start after the initial % character,
// and is returned after the final character denoting the type.
// Based on Wine's implementation for msvcrt.dll (dlls/msvcrt/printf.h).
const char* printf_format_parse(printf_format_t *fmt, const char *p);
/// -------------------------------

// Returns a diagnostic (!) string describing the currently running Windows or
// Wine version, regardless of any manifests.
const char* windows_version(void);
