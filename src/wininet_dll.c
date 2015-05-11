/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * wininet.dll functions.
  */

#include "win32_utf8.h"

const w32u8_pair_t wininet_pairs[] = {
	{"InternetCombineUrlA", InternetCombineUrlU},
	{"InternetCrackUrlA", InternetCrackUrlU},
	{"InternetOpenUrlA", InternetOpenUrlU},
	NULL
};

BOOL WINAPI InternetCombineUrlU(
	LPCSTR lpszBaseUrl,
	LPCSTR lpszRelativeUrl,
	LPSTR lpszBuffer,
	LPDWORD lpdwBufferLength,
	DWORD dwFlags
)
{
	BOOL ret = FALSE;
	if(lpdwBufferLength) {
		DWORD last_error;
		WCHAR_T_DEC(lpszBaseUrl);
		WCHAR_T_DEC(lpszRelativeUrl);
		VLA(wchar_t, lpszBuffer_w, *lpdwBufferLength);

		if(!lpszBuffer) {
			lpszBuffer_w = NULL;
		}
		WCHAR_T_CONV_VLA(lpszBaseUrl);
		WCHAR_T_CONV_VLA(lpszRelativeUrl);
		ret = InternetCombineUrlW(
			lpszBaseUrl_w, lpszRelativeUrl_w, lpszBuffer_w, lpdwBufferLength, dwFlags
		);
		/**
		  * "If the function succeeds, this parameter receives the size of the
		  * combined URL, in characters, not including the null-terminating character.
		  * If the function fails, this parameter receives the size of the required buffer,
		  * in characters (including the null-terminating character).
		  * (http://msdn.microsoft.com/en-us/library/windows/desktop/aa384355%28v=vs.85%29.aspx)
		  */
		if(ret) {
			(*lpdwBufferLength)++;
		}
		last_error = GetLastError();
		if(lpszBuffer) {
			*lpdwBufferLength = StringToUTF8(lpszBuffer, lpszBuffer_w, *lpdwBufferLength);
		} else {
			// Hey, let's be nice and return the _actual_ length.
			VLA(wchar_t, lpszBufferReal_w, *lpdwBufferLength);
			InternetCombineUrlW(
				lpszBaseUrl_w, lpszRelativeUrl_w, lpszBuffer_w, lpdwBufferLength, dwFlags
			);
			ret = StringToUTF8(NULL, lpszBufferReal_w, 0);
			VLA_FREE(lpszBufferReal_w);
		}
		WCHAR_T_FREE(lpszBaseUrl);
		WCHAR_T_FREE(lpszRelativeUrl);
		VLA_FREE(lpszBuffer_w);
		SetLastError(last_error);
	}
	return ret;
}

// Xtreme Token Pasting™
#define UC_SET_W(elm) \
	if(lpUC->lpsz##elm) { \
		VLA(wchar_t, lpsz##elm##_w, lpUC->dw##elm##Length + 1); \
		lpUC_w.lpsz##elm = lpsz##elm##_w; \
	}

#define UC_CONVERT_AND_FREE(elm) \
	if(lpUC->lpsz##elm) { \
		StringToUTF8(lpUC->lpsz##elm, lpUC_w.lpsz##elm, lpUC->dw##elm##Length	); \
		VLA_FREE(lpUC_w.lpsz##elm); \
	}

#define UC_MACRO_EXPAND(macro) \
	macro(Scheme); \
	macro(HostName); \
	macro(UserName); \
	macro(Password); \
	macro(UrlPath); \
	macro(ExtraInfo)

BOOL WINAPI InternetCrackUrlU(
	LPCSTR lpszUrl,
	DWORD dwUrlLength,
	DWORD dwFlags,
	LPURL_COMPONENTSA lpUC
)
{
	BOOL ret = FALSE;
	if(lpUC && lpszUrl) {
		DWORD last_error;
		URL_COMPONENTSW lpUC_w;
		WCHAR_T_DEC(lpszUrl);

		if(dwUrlLength == 0) {
			dwUrlLength = lpszUrl_len;
		}

		memcpy(&lpUC_w, lpUC, lpUC->dwStructSize);
		UC_MACRO_EXPAND(UC_SET_W);

		WCHAR_T_CONV_VLA(lpszUrl);
		ret = InternetCrackUrlW(lpszUrl_w, dwUrlLength, dwFlags, &lpUC_w);

		last_error = GetLastError();
		UC_MACRO_EXPAND(UC_CONVERT_AND_FREE);

		WCHAR_T_FREE(lpszUrl);
		SetLastError(last_error);
	}
	return ret;
}

HINTERNET WINAPI InternetOpenUrlU(
	HINTERNET hInternet,
	LPCSTR lpszUrl,
	LPCSTR lpszHeaders,
	DWORD dwHeadersLength,
	DWORD dwFlags,
	DWORD_PTR dwContext
)
{
	HINTERNET ret = NULL;
	if(dwHeadersLength == -1) {
		dwHeadersLength = strlen(lpszHeaders) + 1;
	}
	if(lpszUrl) {
		WCHAR_T_DEC(lpszUrl);
		VLA(wchar_t, lpszHeaders_w, dwHeadersLength);
		WCHAR_T_CONV_VLA(lpszUrl);
		StringToUTF16(lpszHeaders_w, lpszHeaders, dwHeadersLength);
		ret = InternetOpenUrlW(
			hInternet, lpszUrl_w, lpszHeaders ? lpszHeaders_w : NULL, dwHeadersLength, dwFlags, dwContext
		);
		WCHAR_T_FREE(lpszHeaders);
		WCHAR_T_FREE(lpszUrl);
	}
	return ret;
}
