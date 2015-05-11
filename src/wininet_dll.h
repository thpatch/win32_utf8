/**
  * Win32 UTF-8 wrapper
  *
  * ----
  *
  * wininet.dll functions.
  */

#pragma once

BOOL WINAPI InternetCombineUrlU(
	LPCSTR lpszBaseUrl,
	LPCSTR lpszRelativeUrl,
	LPSTR lpszBuffer,
	LPDWORD lpdwBufferLength,
	DWORD dwFlags
);
#undef InternetCombineUrl
#define InternetCombineUrl InternetCombineUrlU

BOOL WINAPI InternetCrackUrlU(
	LPCSTR lpszUrl,
	DWORD dwUrlLength,
	DWORD dwFlags,
	LPURL_COMPONENTSA lpUrlComponents
);
#undef InternetCrackUrl
#define InternetCrackUrl InternetCrackUrlU

HINTERNET WINAPI InternetOpenUrlU(
	HINTERNET hInternet,
	LPCSTR lpszUrl,
	LPCSTR lpszHeaders,
	DWORD dwHeadersLength,
	DWORD dwFlags,
	DWORD_PTR dwContext
);
#undef InternetOpenUrl
#define InternetOpenUrl InternetOpenUrlU
