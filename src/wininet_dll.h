/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * wininet.dll functions.
  */

#pragma once

WRAPPER_DEC(BOOL WINAPI, InternetCombineUrl,
	LPCSTR lpszBaseUrl,
	LPCSTR lpszRelativeUrl,
	LPSTR lpszBuffer,
	LPDWORD lpdwBufferLength,
	DWORD dwFlags
);
#undef InternetCombineUrl
#define InternetCombineUrl InternetCombineUrlU

WRAPPER_DEC(BOOL WINAPI, InternetCrackUrl,
	LPCSTR lpszUrl,
	DWORD dwUrlLength,
	DWORD dwFlags,
	LPURL_COMPONENTSA lpUrlComponents
);
#undef InternetCrackUrl
#define InternetCrackUrl InternetCrackUrlU

WRAPPER_DEC(HINTERNET WINAPI, InternetOpenUrl,
	HINTERNET hInternet,
	LPCSTR lpszUrl,
	LPCSTR lpszHeaders,
	DWORD dwHeadersLength,
	DWORD dwFlags,
	DWORD_PTR dwContext
);
#undef InternetOpenUrl
#define InternetOpenUrl InternetOpenUrlU
