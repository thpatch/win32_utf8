/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * wininet.dll functions.
  */

const w32u8_pair_t wininet_pairs[] = {
	{"InternetCombineUrlA", InternetCombineUrlU},
	{"InternetCrackUrlA", InternetCrackUrlU},
	{"InternetOpenA", InternetOpenU},
	{"InternetOpenUrlA", InternetOpenUrlU},
	{ NULL }
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
		DWORD len_w = *lpdwBufferLength;
		VLA(wchar_t, lpszBuffer_w, *lpdwBufferLength);

		if(!lpszBuffer) {
			lpszBuffer_w = NULL;
		}
		WCHAR_T_CONV(lpszBaseUrl);
		WCHAR_T_CONV(lpszRelativeUrl);
		ret = InternetCombineUrlW(
			lpszBaseUrl_w, lpszRelativeUrl_w, lpszBuffer_w, &len_w, dwFlags
		);
		/**
		  * "If the function succeeds, this parameter receives the size of the
		  * combined URL, in characters, not including the null-terminating character.
		  * If the function fails, this parameter receives the size of the required buffer,
		  * in characters (including the null-terminating character).
		  * (http://msdn.microsoft.com/en-us/library/windows/desktop/aa384355%28v=vs.85%29.aspx)
		  */
		if(ret) {
			len_w++;
		}
		last_error = GetLastError();
		if(lpszBuffer) {
			*lpdwBufferLength = StringToUTF8(lpszBuffer, lpszBuffer_w, *lpdwBufferLength);
		} else {
			// Hey, let's be nice and return the _actual_ length.
			VLA(wchar_t, lpszBufferReal_w, len_w);
			InternetCombineUrlW(
				lpszBaseUrl_w, lpszRelativeUrl_w, lpszBufferReal_w, &len_w, dwFlags
			);
			// + 1 for the reason explained above.
			ret = StringToUTF8(NULL, lpszBufferReal_w, 0) + 1;
			VLA_FREE(lpszBufferReal_w);
		}
		WCHAR_T_FREE(lpszBaseUrl);
		WCHAR_T_FREE(lpszRelativeUrl);
		VLA_FREE(lpszBuffer_w);
		SetLastError(last_error);
	}
	return ret;
}

#define UC_SET_W(elm) \
	if(lpUC->lpsz##elm) { \
		lpUC_w.lpsz##elm = w32u8_alloca(wchar_t, lpUC->dw##elm##Length + 1); \
	}

#define UC_CONVERT_AND_FREE(elm) \
	if(lpUC->lpsz##elm) { \
		StringToUTF8(lpUC->lpsz##elm, lpUC_w.lpsz##elm, lpUC->dw##elm##Length	); \
		w32u8_freea(lpUC_w.lpsz##elm); \
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

		WCHAR_T_CONV(lpszUrl);
		ret = InternetCrackUrlW(lpszUrl_w, dwUrlLength, dwFlags, &lpUC_w);

		last_error = GetLastError();
		UC_MACRO_EXPAND(UC_CONVERT_AND_FREE);

		WCHAR_T_FREE(lpszUrl);
		SetLastError(last_error);
	}
	return ret;
}

HINTERNET WINAPI InternetOpenU(
	LPCSTR lpszAgent,
	DWORD dwAccessType,
	LPCSTR lpszProxy,
	LPCSTR lpszProxyBypass,
	DWORD dwFlags
)
{
	size_t agent_len = strlen(lpszAgent) + 1;
	size_t proxy_len = lpszProxy ? strlen(lpszProxy) + 1 : 0;
	size_t proxy_bypass_len = lpszProxyBypass ? strlen(lpszProxyBypass) + 1 : 0;
	size_t total_len = agent_len + proxy_len + proxy_bypass_len;
	VLA(wchar_t, param_buffers, total_len);
	wchar_t* param_buffer_write = param_buffers;

	size_t written = StringToUTF16(param_buffer_write, lpszAgent, agent_len);
	lpszAgent = (LPCSTR)param_buffer_write;
	param_buffer_write += written;
	if (lpszProxy) {
		written = StringToUTF16(param_buffer_write, lpszProxy, proxy_len);
		lpszProxy = (LPCSTR)param_buffer_write;
		param_buffer_write += written;
	}
	if (lpszProxyBypass) {
		StringToUTF16(param_buffer_write, lpszProxyBypass, proxy_bypass_len);
		lpszProxyBypass = (LPCSTR)param_buffer_write;
	}

	HINTERNET ret = InternetOpenW(
		(LPCWSTR)lpszAgent, dwAccessType, (LPCWSTR)lpszProxy, (LPCWSTR)lpszProxyBypass, dwFlags
	);

	VLA_FREE(param_buffers);
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
	if(dwHeadersLength == -1 && lpszHeaders) {
		dwHeadersLength = strlen(lpszHeaders) + 1;
	}
	if(lpszUrl) {
		WCHAR_T_DEC(lpszUrl);
		WCHAR_T_CONV(lpszUrl);

		wchar_t* lpszHeaders_w = NULL;
		if (lpszHeaders) {
			lpszHeaders_w = (wchar_t*)w32u8_alloca(wchar_t, dwHeadersLength);
			dwHeadersLength = StringToUTF16(lpszHeaders_w, lpszHeaders, dwHeadersLength);
		}
		ret = InternetOpenUrlW(
			hInternet, lpszUrl_w, lpszHeaders_w, dwHeadersLength, dwFlags, dwContext
		);
		WCHAR_T_FREE(lpszUrl);
		if (lpszHeaders_w) {
			w32u8_freea(lpszHeaders_w);
		}
	}
	return ret;
}
