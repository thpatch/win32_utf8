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

// You might think this is not necessary if your [lpszAgent] only contains
// ASCII characters anyway. However, in contrast to every other Windows API,
// wininet actually uses narrow strings internally, with the encoding being
// determined by this function – InternetOpenA() sets it to the ANSI codepage,
// InternetOpenW() sets it to UTF-8.
// Despite that, InternetOpenUrlW() *always* converts the URL to UTF-8 for
// InternetOpenUrlA(), which then assumes the ANSI codepage for further
// processing. Have fun with the bug reports if that codepage happens to
// be a DBCS one.
//
// (Wine, however, does use a traditional implementation where the A function
// wraps the W function, in case you're now thinking that InternetOpenUrlU()
// is useless if you are sure that your URLs are UTF-8...)
WRAPPER_DEC(HINTERNET WINAPI, InternetOpen,
	LPCSTR lpszAgent,
	DWORD dwAccessType,
	LPCSTR lpszProxy,
	LPCSTR lpszProxyBypass,
	DWORD dwFlags
);
#undef InternetOpen
#define InternetOpen InternetOpenU

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
